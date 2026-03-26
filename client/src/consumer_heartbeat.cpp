#include "consumer_heartbeat.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "sync_group_meta.h"
#include "consumer_coordinator.h"
#include "datahub_client_factory.h"
#include "datahub/datahub_exception.h"
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_config.h"
#include <vector>
#include <algorithm>
#include <pthread.h>

namespace aliyun
{
namespace datahub
{

ConsumerHeartbeat::ConsumerHeartbeat(const std::string& projectName, const std::string& topicName, const std::string& subId, int64_t sessionTimeout,
    const Configuration& conf, ConsumerCoordinator* coordinatorPtr)
    : mClosed(false),
      mConsumerNotFound(false),
      mProjectName(projectName),
      mTopicName(topicName),
      mSubId(subId),
      mConsumerId(coordinatorPtr->GetConsumerId()),
      mVersionId(coordinatorPtr->GetVersionId()),
      mSessionTimeout(sessionTimeout)
{
    mLoggerPtr = Logger::GetInstance().GenLogger("heartbeat");
    mCoordinatorPtr = coordinatorPtr;
    mUniqKey = mCoordinatorPtr->GetUniqKey();
    mHeartbeatTimeout = mSessionTimeout / 6;

    mTimer.reset(new Timer(MIN_TIMEOUT_FOR_START));
    mClientPtr = DatahubClientFactory::GetInstance().GetDatahubClient(conf);

    Start();
}

ConsumerHeartbeat::~ConsumerHeartbeat()
{
    mClosed = true;
    if (mHeartbeatThread.joinable())
    {
        mHeartbeatThread.join();
    }
    LOG_INFO(mLoggerPtr, "Close HeartBeat success. key: %s", mUniqKey.c_str());
}

bool ConsumerHeartbeat::NeedRejoin() const
{
    if (mConsumerNotFound)
    {
        return true;
    }
    int64_t elapsed = mTimer->Elapsed();
    bool isExpire = elapsed > mSessionTimeout;
    if (isExpire)
    {
        LOG_WARN(mLoggerPtr, "ConsumerHeartbeat timeout. key: %s, elapsedMs: %ld, sessionTimeoutMs: %ld", mUniqKey.c_str(), elapsed, mSessionTimeout);
    }
    return isExpire;
}

bool ConsumerHeartbeat::WaitingShardAssign() const
{
    return !HasValidShards();
}

void ConsumerHeartbeat::Start()
{
    std::function<void()> func = std::bind(&ConsumerHeartbeat::KeepHeartbeat, this);
    mHeartbeatThread = std::thread(func);
}

void ConsumerHeartbeat::KeepHeartbeat()
{
    LOG_INFO(mLoggerPtr, "ConsumerHeartbeat start. key: %s, sessionTimeoutMs: %ld, heartbeatTimeoutMs: %ld", mUniqKey.c_str(), mSessionTimeout, mHeartbeatTimeout);
    while (!mClosed)
    {
        if (mTimer->IsExpired())
        {
            std::lock_guard<std::mutex> lock(mMutex);
            HeartbeatOnce();
            if (!HasValidShards())
            {
                LOG_WARN(mLoggerPtr, "Heartbeat has not assign consumer plan, please wait. key: %s", mUniqKey.c_str());
                mTimer->Reset(mHeartbeatTimeout);
            }
            else
            {
                mTimer->Reset(MIN_TIMEOUT_FOR_START);
            }
        }
        else
        {
            try
            {
                mTimer->WaitExpire();
            }
            catch (const std::exception& e)
            {
                LOG_WARN(mLoggerPtr, "Heartbeat wait fail. key: %s, %s", mUniqKey.c_str(), e.what());
                break;
            }
        }
    }
    LOG_INFO(mLoggerPtr, "ConsumerHeartbeat stop. key: %s, sessionTimeoutMs: %ld, heartbeatTimeoutMs: %ld", mUniqKey.c_str(), mSessionTimeout, mHeartbeatTimeout);
}

void ConsumerHeartbeat::HeartbeatOnce()
{
    if (!mClosed)
    {
        try
        {
            const StringVec& holdShards = mCurrShards;
            const StringVec& readEndShards = mCoordinatorPtr->GetSyncGroupMetaPtr()->GetReadEndShards();
            mHeartbeatResult = mClientPtr->Heartbeat(mProjectName, mTopicName, mSubId, mConsumerId, mVersionId, holdShards, readEndShards);
            LOG_DEBUG(mLoggerPtr, "Heartbeat once success. key: %s, holdShards: %s, readEndShards: %s, assignShard: %s",
                mUniqKey.c_str(), PrintUtil::GetMsg(holdShards).c_str(), PrintUtil::GetMsg(readEndShards).c_str(), PrintUtil::GetMsg(mHeartbeatResult.GetShardList()).c_str());

            OnShardChange(mCurrShards, mHeartbeatResult.GetShardList());
            mCurrShards = mHeartbeatResult.GetShardList();

            mCoordinatorPtr->GetSyncGroupMetaPtr()->OnHeartBeatDone(mHeartbeatResult.GetShardList());
            LOG_INFO(mLoggerPtr, "Consumer heartbeat success. key: %s, version: %ld, planVersion: %ld", mUniqKey.c_str(), mVersionId, mHeartbeatResult.GetPlanVersion());
        }
        catch (const DatahubException& e)
        {
            if (e.GetErrorCode() == "OffsetReseted")       // SubscriptionOffsetResetException
            {
                LOG_WARN(mLoggerPtr, "Consumer heartbeat fail, offset reset. key: %s", mUniqKey.c_str());
                mCoordinatorPtr->OnOffsetReset();
            }
            else if (e.GetErrorCode() == "NoSuchSubscription")
            {
                LOG_WARN(mLoggerPtr, "Consumer heartbeat fail, subscription deleted. key: %s", mUniqKey.c_str());
                mCoordinatorPtr->OnSubDeleted();
            }
            else if (e.GetErrorCode() == "NoSuchConsumer")
            {
                LOG_WARN(mLoggerPtr, "Consumer heartbeat fail, consumer deleted. key: %s", mUniqKey.c_str());
                mConsumerNotFound = true;
            }
            else
            {
                LOG_WARN(mLoggerPtr, "Consumer heartbeat fail. key: %s. DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
                throw;
            }
        }
        catch (const std::exception& e){
            LOG_WARN(mLoggerPtr, "Consumer heartbeat fail. key: %s. std::exception: %s", mUniqKey.c_str(), e.what());
            throw;
        }
    }
}

void ConsumerHeartbeat::OnShardChange(const StringVec& oldShards, const StringVec& newShards)
{
    StringVec addShards, delShards;
    for (auto it = newShards.begin(); it != newShards.end(); it++)
    {
        if (std::find(oldShards.begin(), oldShards.end(), *it) == oldShards.end())
        {
            addShards.push_back(*it);
        }
    }
    for (auto it = oldShards.begin(); it != oldShards.end(); it++)
    {
        if (std::find(newShards.begin(), newShards.end(), *it) == newShards.end())
        {
            delShards.push_back(*it);
        }
    }
    if (!addShards.empty() || !delShards.empty())
    {
        mCoordinatorPtr->OnShardChange(addShards, delShards);
        LOG_INFO(mLoggerPtr, "Consumer heartbeat with plan change. key: %s, version: %ld, planVersion: %ld, oldShards: %s, newShards: %s",
            mCoordinatorPtr->GetUniqKey().c_str(), mVersionId, mHeartbeatResult.GetPlanVersion(), PrintUtil::GetMsg(oldShards).c_str(), PrintUtil::GetMsg(newShards).c_str());
    }
}

bool ConsumerHeartbeat::HasValidShards() const
{
    return !mCoordinatorPtr->GetSyncGroupMetaPtr()->ObtainValidShards().empty();
}

} // namespace datahub
} // namespace aliyun
