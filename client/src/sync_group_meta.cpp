#include "sync_group_meta.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include <algorithm>


namespace aliyun
{
namespace datahub
{

SyncGroupMeta::SyncGroupMeta()
{
    mTimer.reset(new Timer(SYNC_GROUP_MIN_INTERVAL));
    mLoggerPtr = Logger::GetInstance().GenLogger("syncGroup");
}

void SyncGroupMeta::OnShardRelease(const StringVec& shardIds)
{
    std::lock_guard<std::mutex> lock(mMutex);
    RemoveAll(shardIds, mReadEndShards);
    RemoveAll(shardIds, mActiveShards);
    AddAll(shardIds, mReleaseShards);
    LOG_INFO(mLoggerPtr, "OnShardRealse for SyncGroupMeta. shards: %s", PrintUtil::GetMsg(shardIds).c_str());
}

void SyncGroupMeta::OnShardReadEnd(const StringVec& shardIds)
{
    std::lock_guard<std::mutex> lock(mMutex);
    AddAll(shardIds, mReadEndShards);
    mTimer->ResetDeadline();
    LOG_INFO(mLoggerPtr, "OnShardReadEnd for SyncGroupMeta. shards: %s", PrintUtil::GetMsg(shardIds).c_str());
}

void SyncGroupMeta::ClearShardRelease()
{
    mReleaseShards.clear();
}

void SyncGroupMeta::OnHeartBeatDone(const StringVec& shardIds)
{
    std::lock_guard<std::mutex> lock(mMutex);
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (std::find(mReadEndShards.begin(), mReadEndShards.end(), *it) == mReadEndShards.end())
        {
            mActiveShards.push_back(*it);
        }
    }
    LOG_DEBUG(mLoggerPtr, "OnHeartBeatDone for SyncGroupMeta. shards: %s", PrintUtil::GetMsg(shardIds).c_str());
}

void SyncGroupMeta::OnSyncDone()
{
    mTimer->Reset();
}

StringVec SyncGroupMeta::ObtainValidShards()
{
    std::lock_guard<std::mutex> lock(mMutex);
    StringVec validShards;
    for (auto it = mActiveShards.begin(); it != mActiveShards.end(); it++)
    {
        if (std::find(mReadEndShards.begin(), mReadEndShards.end(), *it) == mReadEndShards.end())
        {
            validShards.push_back(*it);
        }
    }
    return validShards;
}

bool SyncGroupMeta::NeedSyncGroup()
{
    return mTimer->IsExpired() && !(mReleaseShards.empty() && mReadEndShards.empty());
}

void SyncGroupMeta::AddAll(const StringVec& src, StringVec& tar)
{
    for (auto it = src.begin(); it != src.end(); it++)
    {
        if (std::find(tar.begin(), tar.end(), *it) == tar.end())
        {
            tar.push_back(*it);
        }
    }
}

void SyncGroupMeta::RemoveAll(const StringVec& src, StringVec& tar)
{
    for (auto it = src.begin(); it != src.end(); it++)
    {
        auto tmp = std::find(tar.begin(), tar.end(), *it);
        if (tmp != tar.end())
        {
            tar.erase(tmp);
        }
    }
}

} // namespace datahub
} // namespace aliyun
