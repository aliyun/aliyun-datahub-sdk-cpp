#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_COORDINATOR_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_COORDINATOR_H

#include "client/common_config.h"
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_config.h"
#include <functional>

namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;
class MetaData;
typedef std::shared_ptr<MetaData> MetaDataPtr;

class ShardCoordinator
{
public:
    ShardCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const CommonConfiguration& commonConf);
    virtual ~ShardCoordinator();

    bool IsShardAssign() const;
    void SetAssignShards(const StringVec& shardIds);
    void DoShardChange(const StringVec& addShards, const StringVec& delShards);
    void DoRemoveAllShards();

    void RegisterOnShardChange(std::function<void(const StringVec&, const StringVec&)> func);
    void RegisterOnRemoveAllShards(std::function<void()> func);

    std::string GetUniqKey() const { return mUniqKey; };
    std::string GetProjectName() const { return mProjectName; };
    std::string GetTopicName() const { return mTopicName; };
    std::string GetSubId() const { return mSubId; };
    const CommonConfiguration& GetCommonConfiguration() const { return mCommonConf; };
    const StringVec& GetAssignShards() const { return mAssignShards; };
    MetaDataPtr GetMetaData() const { return mMetaDataPtr; };
    virtual bool WaitShardAssign() const { return false; }
    virtual void UpdateShardInfo();

protected:
    void GenUniqKey(const std::string& suffix="");

protected:
    bool mClosed;
    std::string mProjectName;
    std::string mTopicName;
    std::string mSubId;
    std::string mUniqKey;
    CommonConfiguration mCommonConf;
    StringVec mAssignShards;

    MetaDataPtr mMetaDataPtr;

    std::function<void(const StringVec&, const StringVec&)> mOnShardChangeFunc;
    std::function<void()> mOnRemoveAllShardsFunc;

    log4cpp::Category* mLoggerPtr;
    DatahubClientPtr mClientPtr;
};

typedef std::shared_ptr<ShardCoordinator> ShardCoordinatorPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_COORDINATOR_H
