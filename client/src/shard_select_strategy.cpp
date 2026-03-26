#include "shard_select_strategy.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "shard_offset_heap.h"
#include "datahub/datahub_record.h"
#include <algorithm>

namespace aliyun
{
namespace datahub
{

// ShardReaderSelectStrategy
ShardReaderSelectStrategy::ShardReaderSelectStrategy()
{
    mTimer.reset(new Timer(CFG_READER_SELECT_EMPTY_SHARD_TIMEOUT_MS));
    mLoggerPtr = Logger::GetInstance().GenLogger("ShardReaderSelector");
}

void ShardReaderSelectStrategy::AddShard(const std::string& shardId, int64_t systemTime)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mEmptyShards.erase(shardId);
    OffsetInfoPtr offsetInfo(new OffsetInfo(shardId, systemTime));
    mShardOffsetMap[shardId] = offsetInfo;
    mShardOffsetInfos.Push(offsetInfo);
    LOG_DEBUG(mLoggerPtr, "ShardReaderSelectStrategy add shard. shardId: %s, systemTime: %ld", shardId.c_str(), systemTime);
}

void ShardReaderSelectStrategy::RemoveShard(const std::string& shardId)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mEmptyShards.erase(shardId);
    auto it = mShardOffsetMap.find(shardId);
    if (it != mShardOffsetMap.end())
    {
        it->second->mValid = false;
        mShardOffsetMap.erase(shardId);
    }
    LOG_DEBUG(mLoggerPtr, "ShardReaderSelectStrategy remove shard. shardId: %s", shardId.c_str());
}

std::string ShardReaderSelectStrategy::GetNextShard()
{
    std::lock_guard<std::mutex> lock(mMutex);
    std::string oldestShardId;

    while (!mShardOffsetInfos.Empty())
    {
        const OffsetInfoPtr& offset = mShardOffsetInfos.Top();
        if (offset->mValid)
        {
            oldestShardId = offset->mShardId;
            break ;
        }
        else
        {
            offset->mIndex = -1;
            mShardOffsetInfos.Pop();
        }
    }

    if (oldestShardId.empty() || mTimer->IsExpired())
    {
        LOG_DEBUG(mLoggerPtr, "EmptyShards clear & Timer reset. OldestShardId: %s, EmptyShards: %s", oldestShardId.c_str(), PrintUtil::GetMsg(mEmptyShards).c_str());
        for (const std::string& shard : mEmptyShards)
        {
            auto it = mShardOffsetMap.find(shard);
            if (it != mShardOffsetMap.end())
            {
                it->second->mValid = true;
                mShardOffsetInfos.Push(it->second);
            }
        }
        mEmptyShards.clear();
        mTimer->Reset();
    }

    LOG_DEBUG(mLoggerPtr, "Select shard %s, EmptyShards: %s", oldestShardId.c_str(), PrintUtil::GetMsg(mEmptyShards).c_str());
    return oldestShardId;
}

void ShardReaderSelectStrategy::AfterRead(const std::string& shardId, RecordResultPtr recordPtr)
{
    std::lock_guard<std::mutex> lock(mMutex);
    auto it = mShardOffsetMap.find(shardId);
    if (it == mShardOffsetMap.end())
    {
        return ;
    }

    const OffsetInfoPtr& info = it->second;
    if (recordPtr != nullptr)
    {
        mShardOffsetInfos.Update(info, recordPtr->GetSystemTime());
    }
    else
    {
        mEmptyShards.insert(shardId);
        info->mValid = false;
        LOG_DEBUG(mLoggerPtr, "Record is null, add to emptyShards. shardId: %s", shardId.c_str());
    }
}

// ShardWriterSelectStrategy
ShardWriterSelectStrategy::ShardWriterSelectStrategy() : mShardIndex(-1l)
{
    mTimer.reset(new Timer(CFG_READER_SELECT_EMPTY_SHARD_TIMEOUT_MS));
    mLoggerPtr = Logger::GetInstance().GenLogger("ShardWriterSelector");
}

void ShardWriterSelectStrategy::AddShard(const std::string& shardId)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mActiveShards.push_back(shardId);
    mInactivateShards.erase(shardId);
    LOG_DEBUG(mLoggerPtr, "ShardWriterSelectStrategy add shard. shardId: %s", shardId.c_str());
}

void ShardWriterSelectStrategy::RemoveShard(const std::string& shardId)
{
    std::lock_guard<std::mutex> lock(mMutex);
    RemoveShardFromActive(shardId);
    mInactivateShards.erase(shardId);
    LOG_DEBUG(mLoggerPtr, "ShardWriterSelectStrategy remove shard. shardId: %s", shardId.c_str());
}

void ShardWriterSelectStrategy::ClearAllShard()
{
    std::lock_guard<std::mutex> lock(mMutex);
    mActiveShards.clear();
    mInactivateShards.clear();
    LOG_DEBUG(mLoggerPtr, "ShardWriterSelectStrategy clear all shard");
}

std::string ShardWriterSelectStrategy::GetNextShard()
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mActiveShards.empty() || mTimer->IsExpired())
    {
        mActiveShards.insert(mActiveShards.end(), mInactivateShards.begin(), mInactivateShards.end());
        mTimer->Reset();
    }

    if (mActiveShards.empty())
    {
        return "";
    }

    mShardIndex = (mShardIndex + 1) % mActiveShards.size();
    return mActiveShards[mShardIndex];
}

void ShardWriterSelectStrategy::AddInvalidShard(const std::string& shardId)
{
    std::lock_guard<std::mutex> lock(mMutex);
    RemoveShardFromActive(shardId);
    mInactivateShards.insert(shardId);
    // 无效shard在一定时间内无法写入
    mTimer->Reset();
    LOG_DEBUG(mLoggerPtr, "ShardWriterSelectStrategy add invalid shard. shardId: %s", shardId.c_str());
}

void ShardWriterSelectStrategy::RemoveShardFromActive(const std::string& shardId)
{
    while (true)
    {
        auto iter = std::find(mActiveShards.begin(), mActiveShards.end(), shardId);
        if (iter == mActiveShards.end())
        {
            break;
        }
        mActiveShards.erase(iter);
    }
    LOG_DEBUG(mLoggerPtr, "ShardWriterSelectStrategy remove shard from active shard. shardId: %s", shardId.c_str());
}

} // namespace datahub
} // namespace aliyun
