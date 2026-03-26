#include "offset_coordinator.h"
#include "log4cpp/Category.hh"
#include "logger.h"
#include "offset_manager.h"
#include "datahub/datahub_exception.h"
#include "datahub/datahub_client.h"
#include "client/consumer/consumer_config.h"


namespace aliyun
{
namespace datahub
{

OffsetCoordinator::OffsetCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf)
    : ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)consumerConf),
      mSubSessionChanged(false),
      mSubOffline(false),
      mSubDeleted(false),
      mOffsetNotAck(false)
{
    mOffsetReset.store(false);
    mOffsetManagerPtr.reset(new OffsetManager(mProjectName, mTopicName, mSubId, (Configuration)mCommonConf));
    LOG_INFO(mLoggerPtr, "CommitOffset task start success. key: %s", mUniqKey.c_str());
}

OffsetCoordinator::~OffsetCoordinator()
{
    LOG_INFO(mLoggerPtr, "CommitOffset task stop success. key: %s", mUniqKey.c_str());
}

void OffsetCoordinator::OnSubSessionChanged()
{
    mSubSessionChanged = true;
}

void OffsetCoordinator::OnSubOffline()
{
    mSubOffline = true;
}

void OffsetCoordinator::OnSubDeleted()
{
    mSubDeleted = true;
}

void OffsetCoordinator::OnOffsetNotAck()
{
    mOffsetNotAck = true;
}

void OffsetCoordinator::OnShardReadEnd(const StringVec& shardIds)
{
    StringVec empty;
    DoShardChange(empty, shardIds);
}

void OffsetCoordinator::OnOffsetReset()
{
    if (!mOffsetReset.load())
    {
        mOffsetReset.store(true);
        if (IsShardAssign())
        {
            auto shardIds = GetAssignShards();
            DoShardChange(shardIds, shardIds);
        }
        mOffsetReset.store(false);
    }
}

void OffsetCoordinator::UpdateShardInfo()
{
    if (mSubSessionChanged)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Subscription session has changed. key: " + mUniqKey);
    }
    if (mSubOffline)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Subscription offline. key: " + mUniqKey);
    }
    if (mSubDeleted)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Subscription has been deleted. key: " + mUniqKey);
    }
    if (mOffsetNotAck)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Offset has not been updated for a long time. key: " + mUniqKey);
    }
    ShardCoordinator::UpdateShardInfo();
}

std::map<std::string, ConsumeOffsetMeta> OffsetCoordinator::InitAndGetOffsets(const StringVec& shardIds)
{
    try
    {
        auto initResult = mClientPtr->InitSubscriptionOffsetSession(mProjectName, mTopicName, mSubId, shardIds);
        const auto& subscriptionOffsets = initResult.GetOffsets();
        std::map<std::string, ConsumeOffsetMeta> offsetMetas;
        for (auto it = subscriptionOffsets.begin(); it != subscriptionOffsets.end(); it++)
        {
            LOG_INFO(mLoggerPtr, "InitSubscriptionOffsetSession success. key: %s, shard: %s, sequence: %ld, timestamp: %ld, versionId: %ld, sessionId: %ld",
                    mUniqKey.c_str(), it->first.c_str(), it->second.GetSequence(), it->second.GetTimestamp(), it->second.GetVersion(), it->second.GetSessionId());
            int64_t sequence = it->second.GetSequence();
            offsetMetas[it->first] = ConsumeOffsetMeta(
                it->second.GetTimestamp(),
                sequence < 0 ? sequence : sequence + 1,
                it->second.GetVersion(),
                it->second.GetSessionId()
            );
            // 更新OffsetManager的Version & SessionId;
            mOffsetManagerPtr->UpdateOffsetMeta(it->first, it->second.GetVersion(), it->second.GetSessionId());
        }
        return offsetMetas;
    }
    catch (const DatahubException& e)
    {
        LOG_WARN(mLoggerPtr, "Init Offsets fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
        throw;
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "Init Offsets fail. key: %s, %s", mUniqKey.c_str(), e.what());
        throw;
    }
}

void OffsetCoordinator::SendOffsetRequest(MessageKeyPtr messageKeyPtr)
{
    if (messageKeyPtr != nullptr)
    {
        mOffsetManagerPtr->SendOffsetRequest(messageKeyPtr);
    }
}

} // namespace datahub
} // namespace aliyun
