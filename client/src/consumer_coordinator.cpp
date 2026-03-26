#include "consumer_coordinator.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "offset_manager.h"
#include "sync_group_meta.h"
#include "consumer_heartbeat.h"
#include "datahub/datahub_error.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"
#include "client/consumer/consumer_config.h"

namespace aliyun
{
namespace datahub
{

ConsumerCoordinator::ConsumerCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf)
    : OffsetCoordinator(projectName, topicName, subId, consumerConf),
      mSessionTimeout(std::max(consumerConf.GetSessionTimeout(), MIN_CONSUMER_SESSION_TIMEOUT)),
      mHeartbeatPtr(nullptr)
{
    mSyncGroupMetaPtr = std::make_shared<SyncGroupMeta>();
    JoinGroupAndStartHeartBeat();
}

ConsumerCoordinator::~ConsumerCoordinator()
{
    LeaveGroupAndStopHeartBeat();
    LOG_INFO(mLoggerPtr, "Close ConsumerCoordinator success. key: %s", mUniqKey.c_str());
}

bool ConsumerCoordinator::WaitShardAssign() const
{
    if (mClosed)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "ConsumerCoordinator closed. key: " + mUniqKey);
    }
    return mHeartbeatPtr == nullptr || mHeartbeatPtr->WaitingShardAssign();
}

void ConsumerCoordinator::UpdateShardInfo()
{
    OffsetCoordinator::UpdateShardInfo();
    if (mOffsetReset.load() || mHeartbeatPtr == nullptr || mHeartbeatPtr->NeedRejoin())
    {
        try
        {
            LeaveGroupAndStopHeartBeat();
            JoinGroupAndStartHeartBeat();
            mOffsetReset.store(false);
        }
        catch (const DatahubException& e)
        {
            LOG_WARN(mLoggerPtr, "Rejoin group and start heartbeat fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
            throw;
        }
        catch (const std::exception& e)
        {
            LOG_WARN(mLoggerPtr, "Rejoin group and start heartbeat fail. key: %s, %s", mUniqKey.c_str(), e.what());
            throw;
        }
    }
    SyncConsuemrGroup();
}

void ConsumerCoordinator::OnShardChange(const StringVec& addShards, const StringVec& delShards)
{
    ShardCoordinator::DoShardChange(addShards, delShards);
    if (!delShards.empty())
    {
        // 更新SyncGroupMeta信息
        mSyncGroupMetaPtr->OnShardRelease(delShards);
        // 更新offsetManager信息
        if (mOffsetManagerPtr != nullptr)
        {
            mOffsetManagerPtr->OnShardRealese(delShards);
        }
    }
}

void ConsumerCoordinator::OnShardReadEnd(const StringVec& shardIds)
{
    OffsetCoordinator::OnShardReadEnd(shardIds);
    mSyncGroupMetaPtr->OnShardReadEnd(shardIds);
}

void ConsumerCoordinator::OnOffsetReset()
{
    if (!mOffsetReset.load())
    {
        mOffsetReset.store(true);
        // 更新ShardReader信息
        DoRemoveAllShards();
        // 更新offsetManager信息
        if (mOffsetManagerPtr != nullptr)
        {
            mOffsetManagerPtr->OnOffsetReset();
        }
    }
}

void ConsumerCoordinator::JoinGroup()
{
    Timer joinTimer(MAX_JOIN_GROUP_TIMEOUT);
    while (!joinTimer.IsExpired())
    {
        try
        {
            JoinGroupResult joinGroupResult = mClientPtr->JoinGroup(mProjectName, mTopicName, mSubId, mSessionTimeout);
            mConsumerId = joinGroupResult.GetConsumerId();
            mVersionId = joinGroupResult.GetVersionId();
            mSessionTimeout = joinGroupResult.GetSessionTimeout();
            GenUniqKey(mConsumerId);
            LOG_INFO(mLoggerPtr, "JoinGroup success. key: %s, consumerId: %s, versionId: %ld, sessionTimeout: %ld", mUniqKey.c_str(), mConsumerId.c_str(), mVersionId, mSessionTimeout);
            return ;
        }
        catch (const DatahubException& e)
        {
            LOG_WARN(mLoggerPtr, "JoinGroup fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.what());
            if (!ErrorCode::canRetry(e.GetErrorCode()))
            {
                throw ;
            }
        }
        catch (const std::exception& e)
        {
            LOG_WARN(mLoggerPtr, "JoinGroup fail. key: %s", mUniqKey.c_str());
        }

        try
        {
            joinTimer.WaitExpire(1000);
        }
        catch (const std::exception& e)
        {
            LOG_WARN(mLoggerPtr, "Timer wait fail when join group. key: %s, %s", mUniqKey.c_str(), e.what());
            throw;
        }
    }

    throw DatahubException(LOCAL_ERROR_CODE, "JoinGroup timeout. key: " + mUniqKey);
}

void ConsumerCoordinator::StartHeartBeat()
{
    if (mHeartbeatPtr == nullptr)
    {
        mHeartbeatPtr.reset(new ConsumerHeartbeat(mProjectName, mTopicName, mSubId, mSessionTimeout, mCommonConf, this));
        LOG_INFO(mLoggerPtr, "HeartBeat task start success. key: %s, sessiongTimeout: %ld", mUniqKey.c_str(), mSessionTimeout);
    }
}

void ConsumerCoordinator::JoinGroupAndStartHeartBeat()
{
    JoinGroup();
    StartHeartBeat();
    LOG_INFO(mLoggerPtr, "JoinGroup and StartHeartBeat success. key: %s", mUniqKey.c_str());
}

void ConsumerCoordinator::LeaveGroup()
{
    try
    {
        mClientPtr->LeaveGroup(mProjectName, mTopicName, mSubId, mConsumerId, mVersionId);
        LOG_INFO(mLoggerPtr, "LeaveGroup success. key: %s", mUniqKey.c_str());
    }
    catch (const DatahubException& e)
    {
        LOG_WARN(mLoggerPtr, "LeaveGroup fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "LeaveGroup fail. key: %s, %s", mUniqKey.c_str(), e.what());
    }
}

void ConsumerCoordinator::StopHeartBeat()
{
    if (mHeartbeatPtr != nullptr)
    {
        mHeartbeatPtr.reset();
        LOG_INFO(mLoggerPtr, "HeartBeat task stop success. key: %s", mUniqKey.c_str());
    }
}

void ConsumerCoordinator::LeaveGroupAndStopHeartBeat()
{
    LeaveGroup();
    StopHeartBeat();
    LOG_INFO(mLoggerPtr, "LeaveGroup and StopHeartBeat success. key: %s", mUniqKey.c_str());
}

void ConsumerCoordinator::SyncConsuemrGroup()
{
    if (mSyncGroupMetaPtr->NeedSyncGroup())
    {
        try
        {
            mClientPtr->SyncGroup(mProjectName, mTopicName, mSubId, mConsumerId, mVersionId, mSyncGroupMetaPtr->GetReleaseShards(), mSyncGroupMetaPtr->GetReadEndShards());
            mSyncGroupMetaPtr->ClearShardRelease();
            mSyncGroupMetaPtr->OnSyncDone();
            LOG_INFO(mLoggerPtr, "SyncGroup success. key: %s, releaseShard: %s, readEndShards: %s", mUniqKey.c_str(), PrintUtil::GetMsg(mSyncGroupMetaPtr->GetReleaseShards()).c_str(), PrintUtil::GetMsg(mSyncGroupMetaPtr->GetReadEndShards()).c_str());
        }
        catch (const DatahubException& e)
        {
            LOG_WARN(mLoggerPtr, "SyncGroup fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
            throw;
        }
        catch (const std::exception& e)
        {
            LOG_WARN(mLoggerPtr, "SyncGroup fail. key: %s, %s", mUniqKey.c_str(), e.what());
            throw;
        }
    }
}

} // namespace datahub
} // namespace aliyun
