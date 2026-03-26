#include "meta_data.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "constant.h"
#include "shard_meta.h"
#include "message_reader.h"
#include "message_writer.h"
#include "shard_coordinator.h"
#include "datahub_client_factory.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include "datahub/datahub_typedef.h"
#include "client/topic_meta.h"

namespace aliyun
{
namespace datahub
{

MetaData::MetaData(const std::string& projectName, const std::string& topicName, const std::string& subId, const CommonConfiguration& commonConf)
    : mEndpoint(commonConf.GetEndpoint()), mProjectName(projectName), mTopicName(topicName), mSubId(subId), mCommonConf(commonConf)
{
    mLoggerPtr = Logger::GetInstance().GenLogger("metaData");
    mUpdating.store(false);
    mThreadNum = std::max(std::min(mCommonConf.GetAsycFetchThreadNum(), MAX_FETCH_THREAD_NUMBER), MIN_FETCH_THREAD_NUMBER);
    GenUtil::GenKey(mUniqKey, {mEndpoint, mProjectName, mTopicName, subId});
    mClientPtr = DatahubClientFactory::GetInstance().GetDatahubClient((Configuration)mCommonConf);

    if (subId.empty())
    {
        mMessageReader = nullptr;
        mMessageWriter = std::make_shared<MessageWriter>(mThreadNum, mCommonConf.GetQueueLimitNum());
    }
    else
    {
        mMessageReader = std::make_shared<MessageReader>(mThreadNum, mCommonConf.GetQueueLimitNum());
        mMessageWriter = nullptr;
    }
    mUpdateShardTimer = std::make_shared<Timer>();
}

void MetaData::Init()
{
    InitTopicMeta();
    UpdateShardMetaMap();
    mUpdateShardTimer->Reset(UPDATE_SHRAD_META_INTERVAL);
}

void MetaData::InitTopicMeta()
{
    try
    {
        GetTopicResult getTopic = mClientPtr->GetTopic(mProjectName, mTopicName);
        mTopicMeta = std::make_shared<TopicMeta>(mEndpoint, mProjectName, mTopicName, getTopic.GetRecordType(), getTopic.GetRecordSchema());
    }
    catch (const DatahubException& e)
    {
        LOG_WARN(mLoggerPtr, "Updata TopicMeta fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
        throw;
    }
    catch (const std::exception& e)
    {
        LOG_WARN(mLoggerPtr, "Updata TopicMeta fail. key: %s, %s", mUniqKey.c_str(), e.what());
        throw;
    }
    LOG_DEBUG(mLoggerPtr, "Update TopicMeta once success. key: %s", mUniqKey.c_str());
}

void MetaData::UpdateShardMetaMap()
{
    if (mUpdateShardTimer->IsExpired())
    {
        bool expect = false;
        if (mUpdating.compare_exchange_weak(expect, true))
        {
            try
            {
                ListShardResult listShardResult = mClientPtr->ListShard(mProjectName, mTopicName);
                const auto& listShards = listShardResult.GetShards();
                std::map<std::string, ShardMetaPtr> newShardMetaMap;
                for (auto it = listShards.begin(); it != listShards.end(); it++)
                {
                    if (CheckStateReady(it->GetState()))
                    {
                        newShardMetaMap[it->GetShardId()] = std::make_shared<ShardMeta>(it->GetShardId(), it->GetState());
                    }
                }

                if (OnShardChange(newShardMetaMap))
                {
                    mShardMetaMap = newShardMetaMap;
                }
            }
            catch (const DatahubException& e)
            {
                LOG_WARN(mLoggerPtr, "Updata ShardMeta fail. key: %s, DatahubException: %s", mUniqKey.c_str(), e.GetErrorMessage().c_str());
                throw;
            }
            catch(const std::exception& e)
            {
                LOG_WARN(mLoggerPtr, "Updata ShardMeta fail. key: %s, %s", mUniqKey.c_str(), e.what());
                throw;
            }
            LOG_DEBUG(mLoggerPtr, "Update ShardMeta once success. key: %s", mUniqKey.c_str());
            mUpdating.store(false);
        }
        mUpdateShardTimer->Reset();
    }
}

bool MetaData::CheckStateReady(const ShardState& state)
{
    if (mSubId.empty())     // Writer
    {
        return state == ACTIVE;
    }
    else
    {
        return true;        // Reader
    }
}

bool MetaData::OnShardChange(const std::map<std::string, ShardMetaPtr>& newShardMetaMap)
{
    StringVec addShards, delShards;
    bool changed = false;
    for (auto it = newShardMetaMap.begin(); it != newShardMetaMap.end(); it++)
    {
        if (mShardMetaMap.count(it->first) == 0)
        {
            addShards.push_back(it->first);
            continue;
        }
        if (!changed && mShardMetaMap.at(it->first)->GetShardState() != it->second->GetShardState())
        {
            changed = true;
        }
    }
    for (auto it = mShardMetaMap.begin(); it != mShardMetaMap.end(); it++)
    {
        if (newShardMetaMap.count(it->first) == 0)
        {
            delShards.push_back(it->first);
            continue;
        }
        if (!changed && newShardMetaMap.at(it->first)->GetShardState() != it->second->GetShardState())
        {
            changed = true;
        }
    }
    if (!(addShards.empty() && delShards.empty()))
    {
        for (auto it = mCoordinators.begin(); it != mCoordinators.end(); it++)
        {
            (*it)->DoShardChange(addShards, delShards);
        }
        changed = true;
    }
    return changed;
}

void MetaData::UpdateShardOnceRightNow()
{
    if (mUpdateShardTimer != nullptr)
    {
        mUpdateShardTimer->ResetDeadline();
        UpdateShardMetaMap();
    }
}

void MetaData::Register(ShardCoordinator* coordinatorPtr)
{
    mCoordinators.insert(coordinatorPtr);
}

void MetaData::Unregister(ShardCoordinator* coordinatorPtr)
{
    mCoordinators.erase(coordinatorPtr);
}

} // namespace datahub
} // namespace aliyun
