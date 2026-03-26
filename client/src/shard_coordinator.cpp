#include "shard_coordinator.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "logger.h"
#include "meta_data.h"
#include "meta_cache.h"
#include "datahub_client_factory.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include "client/common_config.h"


namespace aliyun
{
namespace datahub
{

ShardCoordinator::ShardCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const CommonConfiguration& commonConf)
    : mClosed(false),
      mProjectName(projectName),
      mTopicName(topicName),
      mSubId(subId),
      mCommonConf(commonConf)
{
    mLoggerPtr = Logger::GetInstance().GenLogger("coordinator");
    GenUniqKey();
    mClientPtr = DatahubClientFactory::GetInstance().GetDatahubClient((Configuration)mCommonConf);
    mMetaDataPtr = MetaCache::GetInstance().GetMetaData(mProjectName, mTopicName, mSubId, this, mCommonConf);
    LOG_DEBUG(mLoggerPtr, "Register metaData success. key: %s", mUniqKey.c_str());
}

ShardCoordinator::~ShardCoordinator()
{
    mClosed = true;
    mMetaDataPtr->Unregister(this);
    LOG_DEBUG(mLoggerPtr, "Close Coordinator success. key: %s", mUniqKey.c_str());
}

bool ShardCoordinator::IsShardAssign() const
{
    return !mAssignShards.empty();
}

void ShardCoordinator::SetAssignShards(const StringVec& shardIds)
{
    const auto& shardMetaMap = mMetaDataPtr->GetShardMetaMap();
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (shardMetaMap.count(*it) == 0)
        {
            LOG_WARN(mLoggerPtr, "ShardId is not exist. key: %s, shardId: %s", mUniqKey.c_str(), it->c_str());
            throw DatahubException(LOCAL_ERROR_CODE, "ShardId is not exist");
        }
    }
    LOG_INFO(mLoggerPtr, "SetAssignShards success. key: %s, shardIds: %s", mUniqKey.c_str(), PrintUtil::GetMsg(shardIds).c_str());
    mAssignShards = shardIds;
}

void ShardCoordinator::DoShardChange(const StringVec& addShards, const StringVec& delShards)
{
    if (mClosed)
    {
        LOG_WARN(mLoggerPtr, "Coordinator closed. key: %s", mUniqKey.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "Coordinator closed. key: " + mUniqKey);
    }
    // For ShardReader change
    if (mOnShardChangeFunc != nullptr && (!addShards.empty() || !delShards.empty()))
    {
        mOnShardChangeFunc(addShards, delShards);
    }
}

void ShardCoordinator::DoRemoveAllShards()
{
    if (mClosed)
    {
        LOG_WARN(mLoggerPtr, "Coordinator closed. key: %s", mUniqKey.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "Coordinator closed. key: " + mUniqKey);
    }
    // For ShardReader change
    if (mOnRemoveAllShardsFunc != nullptr)
    {
        mOnRemoveAllShardsFunc();
    }
}

void ShardCoordinator::GenUniqKey(const std::string& suffix)
{
    if (mUniqKey.empty())
    {
        mUniqKey.append(mProjectName).append(":").append(mTopicName);
        if (!mSubId.empty())
        {
            mUniqKey.append(":").append(mSubId);
        }
    }
    if (!suffix.empty())
    {
        mUniqKey.append(":").append(suffix);
    }
}

void ShardCoordinator::UpdateShardInfo()
{
    if (mClosed)
    {
        LOG_WARN(mLoggerPtr, "Coordinator closed. key: %s", mUniqKey.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "Coordinator closed. key: " + mUniqKey);
    }
    mMetaDataPtr->UpdateShardMetaMap();
}

void ShardCoordinator::RegisterOnShardChange(std::function<void(const StringVec&, const StringVec&)> func)
{
    mOnShardChangeFunc = func;
}

void ShardCoordinator::RegisterOnRemoveAllShards(std::function<void()> func)
{
    mOnRemoveAllShardsFunc = func;
}

} // namespace datahub
} // namespace aliyun
