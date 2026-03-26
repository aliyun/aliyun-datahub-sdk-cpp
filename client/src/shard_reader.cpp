#include "shard_reader.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "message_reader.h"
#include "datahub_client_factory.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include "client/consumer/consumer_config.h"
#include <functional>

namespace aliyun
{
namespace datahub
{

ShardReader::ShardReader(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf,
    const MessageReaderPtr& messageReaderPtr, const std::string& shardId, const ConsumeOffsetMeta& offset, int32_t fetchNum)
    : mClosed(false),
      mFetchNum(fetchNum),
      mRecordFetchQueueLimit(consumerConf.GetRecordFetchQueueLimit()),
      mProjectName(projectName),
      mTopicName(topicName),
      mSubId(subId),
      mShardId(shardId),
      mCurrOffset(offset),
      mMessageReaderPtr(messageReaderPtr)
{
    mFetchTotalNum.store(0);
    mTaskRuning.store(false);
    mLastTimer.reset(new Timer());
    mLoggerPtr = Logger::GetInstance().GenLogger("shardReader-" + mShardId);
    GenUtil::GenKey(mUniqKey, {mProjectName, mTopicName, mSubId});
    mClientPtr = DatahubClientFactory::GetInstance().GetDatahubClient((Configuration)consumerConf);
}

ShardReader::~ShardReader()
{
    mClosed = true;
    LOG_INFO(mLoggerPtr, "Fetch %ld records. key: %s, shardId: %s", mFetchTotalNum.load(), mUniqKey.c_str(), mShardId.c_str());
}

RecordResultPtr ShardReader::Read(int64_t timeout)
{
    if (mClosed)
    {
        LOG_WARN(mLoggerPtr, "ShardReader closed. key: %s, shardId: %s", mUniqKey.c_str(), mShardId.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "ShardReader closed. key: " + mUniqKey + ", shardId: " + mShardId);
    }
    std::unique_lock<std::mutex> lock(mReadMutex);
    auto recordPtr = ReadNext(timeout);
    return recordPtr;
}

void ShardReader::ResetOffset()
{
    mCurrOffset.ResetTimestamp(-1);
    LOG_INFO(mLoggerPtr, "Fetch %ld records before reset offset to -1. key: %s, shardId: %s", mFetchTotalNum.load(), mUniqKey.c_str(), mShardId.c_str());
    mFetchTotalNum.store(0);
}

std::string ShardReader::GetNextCursor() const
{
    if (!mCurrOffset.GetNextCursor().empty())
    {
        return mCurrOffset.GetNextCursor();
    }

    std::string cursor = "";
    CursorType type = CURSOR_TYPE_OLDEST;
    try
    {
        if (mCurrOffset.GetSequence() != -1l)
        {
            type = CURSOR_TYPE_SEQUENCE;
            cursor = ObtainCursor(type, mCurrOffset.GetSequence());
        }
        else if (mCurrOffset.GetTimestamp() != -1l)
        {
            type = CURSOR_TYPE_TIMESTAMP;
            cursor = ObtainCursor(type, mCurrOffset.GetTimestamp());
        }
        else
        {
            type = CURSOR_TYPE_OLDEST;
            cursor = ObtainCursor(type, -1l);
        }
        LOG_INFO(mLoggerPtr, "Init cursor success. key: %s, shardId: %s, type: %d, sequence: %ld, timestamp: %ld, cursor: %s",
            mUniqKey.c_str(), mShardId.c_str(), type, mCurrOffset.GetSequence(), mCurrOffset.GetTimestamp(), cursor.c_str());
    }
    catch (const DatahubException& e)
    {
        if (e.GetErrorCode() == "SeekOutOfRange")
        {
            LOG_WARN(mLoggerPtr, "ShardReader GetCursor fail with type %d, retry by CURSOR_TYPE_OLDEST. key: %s, shardId: %s, sequence: %ld, timestamp: %ld, DatahubException: %s",
                    type, mUniqKey.c_str(), mShardId.c_str(), mCurrOffset.GetSequence(), mCurrOffset.GetTimestamp(), e.GetErrorMessage().c_str());
            cursor = ObtainCursor(CURSOR_TYPE_OLDEST, -1l);
        }
        else
        {
            LOG_WARN(mLoggerPtr, "ShardReader GetCursor fail. key: %s, shardId: %s, DatahubException: %s", mUniqKey.c_str(), mShardId.c_str(), e.GetErrorMessage().c_str());
            throw;
        }
    }
    return cursor;
}

std::string ShardReader::ObtainCursor(const CursorType& type, int64_t value) const
{
    if (type == CURSOR_TYPE_OLDEST || type == CURSOR_TYPE_LATEST)
    {
        return mClientPtr->GetCursor(mProjectName, mTopicName, mShardId, type).GetCursor();
    }
    else
    {
        return mClientPtr->GetCursor(mProjectName, mTopicName, mShardId, type, value).GetCursor();
    }
}

GetRecordResult ShardReader::GetNextRecordResult(const std::string& cursor)
{
    try
    {
        return mClientPtr->GetRecord(mProjectName, mTopicName, mShardId, cursor, mFetchNum, mSubId);
    }
    catch (const DatahubException& e)
    {
        LOG_WARN(mLoggerPtr, "ShardReader GetRecord fail. key: %s, shardId: %s, DatahubException: %s", mUniqKey.c_str(), mShardId.c_str(), e.GetErrorMessage().c_str());
        throw;
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "ShardReader GetRecord fail. key: %s, shardId: %s, %s", mUniqKey.c_str(), mShardId.c_str(), e.what());
        throw;
    }
}

void ShardReader::FetchOnce()
{
    GetRecordResult getRecordResult;
    try
    {
        std::string cursor = GetNextCursor();
        if (!cursor.empty())
        {
            getRecordResult = GetNextRecordResult(cursor);
        }
    }
    catch (const DatahubException& e)
    {
        std::unique_lock<std::mutex> fetchLock(mFetchMutex);
        if (mCompletedFetchCacheQueue.empty() || mCompletedFetchCacheQueue.back().mType != T_EXCEPTION)
        {
            mCompletedFetchCacheQueue.push(CompletedFetch(mShardId, std::current_exception()));
        }
        NotifyAllFetchWait();
        LOG_WARN(mLoggerPtr, "FetchTask throw DatahubException. key: %s, shardId: %s, %s", mUniqKey.c_str(), mShardId.c_str(), e.GetErrorMessage().c_str());
        return ;
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "FetchTask throw exception. key: %s, shardId: %s", mUniqKey.c_str(), mShardId.c_str());
        throw;
    }

    if (getRecordResult.GetRecordCount() > 0)
    {
        std::unique_lock<std::mutex> fetchLock(mFetchMutex);
        std::vector<RecordResult> records = getRecordResult.GetRecords();
        for (auto it = records.begin(); it != records.end(); it++)
        {
            it->SetMessageKey(std::make_shared<MessageKey>(mShardId, getRecordResult.GetNextCursor(), SubscriptionOffset(it->GetSystemTime(), it->GetSequence())));
            mCompletedFetchCacheQueue.push(CompletedFetch(mShardId, *it));
        }
        mCurrOffset.SetNextCursor(getRecordResult.GetNextCursor());
        LOG_DEBUG(mLoggerPtr, "GetRecord once success. key: %s, shardId: %s, remain size: %ld", mUniqKey.c_str(), mShardId.c_str(), mCompletedFetchCacheQueue.size());
    }
    else
    {
        std::unique_lock<std::mutex> fetchLock(mFetchMutex);
        mCompletedFetchCacheQueue.push(CompletedFetch(mShardId, DELAY_TIMEOUT_FOR_READ_END));
        mCurrOffset.SetNextCursor(getRecordResult.GetNextCursor());
        LOG_DEBUG(mLoggerPtr, "Shard read empty, will retry in %ld Ms. key: %s, shardId: %s, cursor: %s", DELAY_TIMEOUT_FOR_READ_END, mUniqKey.c_str(), mShardId.c_str(), getRecordResult.GetNextCursor().c_str());
    }
    NotifyAllFetchWait();
}

RecordResultPtr ShardReader::ReadNext(int64_t timeout)
{
    Timer timer(timeout);
    RecordResultPtr recordPtr = nullptr;

    if (!mLastTimer->IsExpired())
    {
        LOG_INFO(mLoggerPtr, "ShardReader last timer not expired, wait it. key: %s, shardId: %s", mUniqKey.c_str(), mShardId.c_str());
        mLastTimer->WaitExpire();
    }

    do
    {
        if (mCompletedFetchCacheQueue.size() < mRecordFetchQueueLimit && !mTaskRuning.exchange(true))
        {
            SendTaskAndWait(timer);
        }

        std::unique_lock<std::mutex> fetchLock(mFetchMutex);
        if (mCompletedFetchCacheQueue.empty())
        {
            continue ;
        }

        auto tmp = mCompletedFetchCacheQueue.front();
        if (tmp.mType == T_NORMAL)
        {
            mCompletedFetchCacheQueue.pop();
            recordPtr = tmp.mRecordPtr;
            mFetchTotalNum.fetch_add(1);
            break;
        }
        else if (tmp.mType == T_EXCEPTION)
        {
            mCompletedFetchCacheQueue.pop();
            std::rethrow_exception(tmp.mExceptionPtr);
        }
        else
        {
            if (!(tmp.mTimerPtr->IsExpired()))
            {
                mLastTimer->Reset(tmp.mTimerPtr->GetRemainTimeMs());
            }
            mCompletedFetchCacheQueue.pop();
            break ;
        }
    } while (!mClosed && !timer.IsExpired());

    return recordPtr;
}

void ShardReader::SendTaskAndWait(Timer& timer)
{
    SendNextTask();
    try
    {
        std::function<bool()> func = [this]{return !mCompletedFetchCacheQueue.empty();};
        timer.WaitFor(mFetchMutex, mFetchCV, func);
    }
    catch (const DatahubException& e)
    {
    }
}

void ShardReader::SendNextTask()
{
    std::function<void()> func = std::bind(&ShardReader::FetchOnce, this);
    mMessageReaderPtr->SubmitTask(func);
    LOG_DEBUG(mLoggerPtr, "Send next fetch task success. key: %s, shardId: %s", mUniqKey.c_str(), mShardId.c_str());
}

void ShardReader::NotifyAllFetchWait()
{
    mTaskRuning.store(false);
    mFetchCV.notify_all();
}

} // namespace datahub
} // namespace aliyun
