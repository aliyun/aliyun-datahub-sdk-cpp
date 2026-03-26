#include "shard_writer.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "record_pack.h"
#include "record_pack_queue.h"
#include "message_writer.h"
#include "shard_select_strategy.h"
#include "datahub_client_factory.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include "client/write_result.h"
#include "client/producer/producer_config.h"

namespace aliyun
{
namespace datahub
{

ShardWriter::ShardWriter(const std::string& projectName, const std::string& topicName, const ProducerConfiguration& producerConf, const std::string& shardId,
        const MessageWriterPtr messageWriter, const ShardWriterSelectStrategyPtr& shardSelector)
    : mClosed(false),
      mProjectName(projectName),
      mTopicName(topicName),
      mShardId(shardId),
      mRetryTimes(producerConf.GetRetryTimes()),
      mMessageWriter(messageWriter),
      mShardSelector(shardSelector)
{
    mTotalTaskNum.store(0l);
    mWriteTotalNum.store(0);
    mLoggerPtr = Logger::GetInstance().GenLogger("shardWriter-" + mShardId);
    GenUtil::GenKey(mUniqKey, {mProjectName, mTopicName});
    mQueueMutex.reset(new std::mutex());
    mRecordPackQueuePtr = std::make_shared<RecordPackQueue>(producerConf.GetMaxAsyncBufferSize(), producerConf.GetMaxAsyncBufferRecords(), producerConf.GetMaxAsyncBufferTimeMs(), producerConf.GetMaxRecordPackQueueLimit());
    mClientPtr = DatahubClientFactory::GetInstance().GetDatahubClient((Configuration)producerConf);
}

ShardWriter::~ShardWriter()
{
    mClosed = true;
    LOG_INFO(mLoggerPtr, "Write %ld records total. key: %s, shardId: %s", mWriteTotalNum.load(), mUniqKey.c_str(), mShardId.c_str());
}

void ShardWriter::Write(const RecordEntryVec& records)
{
    if (mClosed)
    {
        LOG_WARN(mLoggerPtr, "ShardWriter closed. key: %s, shardId: %s", mUniqKey.c_str(), mShardId.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "ShardWriter closed. key: " + mUniqKey + ", shardId: " + mShardId);
    }
    WriteOnce(records);
    LOG_DEBUG(mLoggerPtr, "Send next write task success. key: %s, record size: %d", mUniqKey.c_str(), records.size());
}

WriteResultFuturePtr ShardWriter::WriteAsync(const RecordEntryVec& records)
{
    if (mClosed)
    {
        LOG_WARN(mLoggerPtr, "ShardWriter closed. key: %s, shardId: %s", mUniqKey.c_str(), mShardId.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "ShardWriter closed. key: " + mUniqKey + ", shardId: " + mShardId);
    }
    WriteResultFuturePtr result = mRecordPackQueuePtr->AppendRecord(records);

    SendNextTask(false);
    return result;
}

void ShardWriter::Flush()
{
    SendNextTask(true);

    std::unique_lock<std::mutex> ul(mTaskMutex);
    std::function<bool()> func = [this]{ return mTotalTaskNum.load() == 0l; };
    mTaskCV.wait(ul, func);
}

void ShardWriter::WriteOnce(const RecordEntryVec& records)
{
    int32_t retryNum = 0;
    while (true)
    {
        try
        {
            mClientPtr->PutRecordByShard(mProjectName, mTopicName, mShardId, records);
            mWriteTotalNum.fetch_add(records.size());
            return ;
        }
        catch(const DatahubException& e)
        {
            LOG_WARN(mLoggerPtr, "Put records fail. key: %s, shardId: %s, records size: %d, max retry time: %d, this time: %d, DatahubException: %s", mUniqKey.c_str(), mShardId.c_str(), records.size(), mRetryTimes, retryNum, e.GetErrorMessage().c_str());
            if (++retryNum >= mRetryTimes)
            {
                throw ;
            }
        }
        catch(const std::exception& e)
        {
            LOG_WARN(mLoggerPtr, "Put records fail. key: %s, shardId: %s, records size: %d, max retry time: %d, this time: %d, %s", mUniqKey.c_str(), mShardId.c_str(), records.size(), mRetryTimes, retryNum, e.what());
            throw ;
        }
    }
}

void ShardWriter::SendNextTask(bool forceFlush)
{
    std::unique_lock<std::mutex> ul(mSendMutex);
    RecordPackPtr readyPack = mRecordPackQueuePtr->ObtainReadyRecordPack(forceFlush);
    if (readyPack)
    {
        std::function<void()> func = std::bind(&ShardWriter::SendTaskOnce, this, readyPack);
        // key 保证同一时刻同一个shard只有一个写入任务，以保证写入的数据有序
        int32_t key = std::stoi(mShardId);
        if (mMessageWriter->SubmitTask(key, func))
        {
            mTotalTaskNum.fetch_add(1);
            mRecordPackQueuePtr->PopBackRecordPack();
        }
    }
}

void ShardWriter::SendTaskOnce(const RecordPackPtr& recordPack)
{
    const auto& records = recordPack->GetRecords();
    const auto& promises = recordPack->GetWriteResults();

    try
    {
        int64_t startSendTimeMs = Timer::GetCurrentTimeMs();
        WriteOnce(records);
        int64_t endSendTimeMs = Timer::GetCurrentTimeMs();

        int64_t elapsedTimeMs = endSendTimeMs - recordPack->GetInitTimeMs();
        int64_t sendTimeMs = endSendTimeMs - startSendTimeMs;

        for (auto reIt = promises.begin(); reIt != promises.end(); reIt++)
        {
            (*reIt)->set_value(std::make_shared<WriteResult>(mShardId, elapsedTimeMs, sendTimeMs));
        }

        TaskDone();
    }
    catch (const DatahubException& e)
    {
        LOG_WARN(mLoggerPtr, "WriteOnceAsync fail. key: %s, shardId: %s, records size: %d, DatahubException: %s", mUniqKey.c_str(), mShardId.c_str(), records.size(), e.GetErrorMessage().c_str());
        mShardSelector->AddInvalidShard(mShardId);
        for (auto reIt = promises.begin(); reIt != promises.end(); reIt++)
        {
            (*reIt)->set_exception(std::current_exception());
        }
        TaskDone();
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "WriteOnceAsync fail. key: %s, shardId: %s, records size: %d, %s", mUniqKey.c_str(), mShardId.c_str(), records.size(), e.what());
        mShardSelector->AddInvalidShard(mShardId);
        for (auto reIt = promises.begin(); reIt != promises.end(); reIt++)
        {
            (*reIt)->set_exception(std::current_exception());
        }
        TaskDone();
    }
}

void ShardWriter::TaskDone()
{
    std::unique_lock<std::mutex> ul(mTaskMutex);
    mTotalTaskNum.fetch_sub(1l);
    mTaskCV.notify_all();
    SendNextTask(false);
}

} // namespace datahub
} // namespace aliyun
