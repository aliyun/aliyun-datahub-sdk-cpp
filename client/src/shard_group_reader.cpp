#include "shard_group_reader.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "meta_data.h"
#include "shard_reader.h"
#include "offset_coordinator.h"
#include "shard_select_strategy.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"
#include "client/consumer/consumer_config.h"


namespace aliyun
{
namespace datahub
{

thread_local MessageKeyPtr ShardGroupReader::sLastMessagekeyPtr = nullptr;

ShardGroupReader::ShardGroupReader(OffsetCoordinator* coordinatorPtr, const ConsumerConfiguration& consumerConf, const StringVec& shardIds, int64_t timestamp)
    : mClosed(false),
      mAutoAckOffset(consumerConf.GetAutoAckOffset()),
      mFetchNum(consumerConf.GetFetchLimitNum()),
      mConsumerConf(consumerConf),
      mCoordinatorPtr(coordinatorPtr)
{
    mLoggerPtr = Logger::GetInstance().GenLogger("shardGroupReader");
    mShardSelector.reset(new ShardReaderSelectStrategy());
    mCoordinatorPtr->SetAssignShards(shardIds);
    CreateShardReader(shardIds, timestamp);
}

ShardGroupReader::~ShardGroupReader()
{
    mClosed = true;
    RemoveAllShardReader();
}

RecordResultPtr ShardGroupReader::Read(const std::string& shardId, int64_t timeout)
{
    if (mClosed)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "ShardGroupReader closed. key: " + mCoordinatorPtr->GetUniqKey());
    }

    if (mAutoAckOffset)
    {
        AckLastMessageKey();
    }

    CheckTimeout(timeout);

    Timer timer(timeout);
    RecordResultPtr recordResult = nullptr;
    do
    {
        if (mCoordinatorPtr->WaitShardAssign())
        {
            timer.WaitExpire(CFG_DELAY_TIMEOUT_FOR_NOT_READY);
        }
        else
        {
            std::unique_lock<std::mutex> lock(mMutex);
            mCoordinatorPtr->UpdateShardInfo();

            const ShardReaderPtr& readerPtr = GetNextReader(shardId);
            if (readerPtr == nullptr)
            {
                timer.WaitExpire(CFG_DELAY_TIMEOUT_FOR_NOT_READY);
            }
            else
            {
                recordResult = ReadOnceByReader(readerPtr);
            }
        }
    } while (!mClosed && recordResult == nullptr && !timer.IsExpired());
    return recordResult;
}

void ShardGroupReader::OnShardChange(const StringVec& addShards, const StringVec& delShards)
{
    LOG_DEBUG(mLoggerPtr, "Callback shard reader change. key: %s, add shardId: %s, del shardIds: %s", mCoordinatorPtr->GetUniqKey().c_str(), PrintUtil::GetMsg(addShards).c_str(), PrintUtil::GetMsg(delShards).c_str());
    CreateShardReader(addShards, -1l);
    RemoveShardReader(delShards);
}

void ShardGroupReader::OnRemoveAllShards()
{
    LOG_DEBUG(mLoggerPtr, "Callback remove all shards reader. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
    RemoveAllShardReader();
}

void ShardGroupReader::CreateShardReader(const StringVec& shardIds, int64_t timestamp)
{
    if (shardIds.empty())
    {
        return ;
    }
    std::unique_lock<std::mutex> lock(mShardReaderMutex);
    auto shardOffsetMap = GenShardOffset(shardIds, timestamp);
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (mShardReaderMap.count(*it) == 0)
        {
            const auto& offset = shardOffsetMap.at(*it);
            // For mShardSelector
            mShardSelector->AddShard(*it);
            // For mShardReaderMap
            mShardReaderMap[*it] = std::make_shared<ShardReader>(mCoordinatorPtr->GetProjectName(), mCoordinatorPtr->GetTopicName(), mCoordinatorPtr->GetSubId(),
                                            mConsumerConf, mCoordinatorPtr->GetMetaData()->GetMessageReader(), *it, offset, mFetchNum);
            LOG_INFO(mLoggerPtr, "ShardReader created. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), it->c_str());
        }
    }
}

void ShardGroupReader::RemoveShardReader(const StringVec& shardIds)
{
    std::unique_lock<std::mutex> lock(mShardReaderMutex);
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (mShardReaderMap.count(*it) > 0)
        {
            // For mShardSelector
            mShardSelector->RemoveShard(*it);
            // For mShardReaderMap
            mShardReaderMap.erase(*it);
            LOG_INFO(mLoggerPtr, "ShardReader removed. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), it->c_str());
        }
    }
}

void ShardGroupReader::RemoveAllShardReader()
{
    std::unique_lock<std::mutex> lock(mShardReaderMutex);
    while (!mShardReaderMap.empty())
    {
        auto tmp = mShardReaderMap.begin();
        // For mShardSelector
        mShardSelector->RemoveShard(tmp->first);
        // For mShardReaderMap
        mShardReaderMap.erase(tmp->first);
        LOG_INFO(mLoggerPtr, "ShardReader removed when remove all shardReader. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), tmp->first.c_str());
    }
}

std::map<std::string, ConsumeOffsetMeta> ShardGroupReader::GenShardOffset(const StringVec& shardIds, int64_t timestamp)
{
    std::map<std::string, ConsumeOffsetMeta> offset = mCoordinatorPtr->InitAndGetOffsets(shardIds);
    if (timestamp != -1)
    {
        for (auto it = offset.begin(); it != offset.end(); it++)
        {
            it->second.ResetTimestamp(timestamp);
        }
    }
    return offset;
}

ShardReaderPtr ShardGroupReader::GetNextReader(const std::string& shardId)
{
    const std::string& nextShardId = shardId.empty() ? mShardSelector->GetNextShard() : shardId;
    if (nextShardId.empty())
    {
        if (mShardReaderMap.empty())
        {
            LOG_WARN(mLoggerPtr, "No active ShardReader found. May the specified shards all closed. key: ", mCoordinatorPtr->GetUniqKey().c_str());
            throw DatahubException(LOCAL_ERROR_CODE, "No active shard found. key: " + mCoordinatorPtr->GetUniqKey());
        }
        else
        {
            return mShardReaderMap.begin()->second;
        }
    }

    return GetShardReader(nextShardId);
}

ShardReaderPtr ShardGroupReader::GetShardReader(const std::string& shardId)
{
    std::lock_guard<std::mutex> lock(mShardReaderMutex);
    auto readerIt = mShardReaderMap.find(shardId);
    if (readerIt != mShardReaderMap.end())
    {
        return readerIt->second;
    }
    else
    {
        LOG_WARN(mLoggerPtr, "ShardReader not found, maybe it has no data. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), shardId.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "ShardReader not found. key: " + mCoordinatorPtr->GetUniqKey() + ", shardId: " + shardId);
    }
}

inline void ShardGroupReader::AckLastMessageKey()
{
    if (sLastMessagekeyPtr != nullptr)
    {
        sLastMessagekeyPtr->Ack();
        sLastMessagekeyPtr = nullptr;
    }
}

// 消费单条数据的等待时间限定在 [MIN_TIMEOUT_WAIT_FOR_GROUP_READ, MAX_TIMEOUT_WAIT_FOR_GROUP_READ] 区间内
inline void ShardGroupReader::CheckTimeout(int64_t& timeout)
{
    if (timeout > MAX_TIMEOUT_WAIT_FOR_GROUP_READ)
    {
        timeout = MAX_TIMEOUT_WAIT_FOR_GROUP_READ;
    }
    else if (timeout < MIN_TIMEOUT_WAIT_FOR_GROUP_READ)
    {
        timeout = MIN_TIMEOUT_WAIT_FOR_GROUP_READ;
    }
}

inline RecordResultPtr ShardGroupReader::ReadOnceByReader(ShardReaderPtr readerPtr)
{
    try
    {
        auto recordPtr = readerPtr->Read(1000);
        mShardSelector->AfterRead(readerPtr->GetShardId(), recordPtr);
        if (recordPtr != nullptr)
        {
            MessageKeyPtr msgKey = recordPtr->GetMessageKey();
            mCoordinatorPtr->SendOffsetRequest(msgKey);
            if (mAutoAckOffset && msgKey != nullptr)
            {
                sLastMessagekeyPtr = msgKey;
            }
            return recordPtr;
        }
    }
    catch (const DatahubException& e)
    {
        if (e.GetErrorCode() == "InvalidCursor")                   // InvalidCursorException
        {
            LOG_WARN(mLoggerPtr, "Obtain invalid cursor exception. key: %s, shardId: %s",mCoordinatorPtr->GetUniqKey().c_str(), readerPtr->GetShardId().c_str());
            readerPtr->ResetOffset();
        }
        else if (e.GetErrorCode() == "InvalidShardOperation")      // ShardSealedException      The specified shard is not active.
        {
            // Change ShardGroupReader, SyncGroupMeta, ShardSelector
            mCoordinatorPtr->OnShardReadEnd({readerPtr->GetShardId()});
            LOG_WARN(mLoggerPtr, "Shard read end. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), readerPtr->GetShardId().c_str());
        }
        else
        {
            LOG_WARN(mLoggerPtr, "ShardGroupReader read fail. key: %s, shardId: %s, DatahubException: %s", mCoordinatorPtr->GetUniqKey().c_str(), readerPtr->GetShardId().c_str(), e.GetErrorMessage().c_str());
            throw;
        }
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "ShardGroupReader read fail. key: %s, shardId: %s, %s", mCoordinatorPtr->GetUniqKey().c_str(), readerPtr->GetShardId().c_str(), e.what());
        throw;
    }
    return nullptr;
}

} // namespace datahub
} // namespace aliyun
