#ifndef INCLUDE_DATAHUB_CLIENT_META_DATA_H
#define INCLUDE_DATAHUB_CLIENT_META_DATA_H

#include "client/common_config.h"
#include "datahub/datahub_shard.h"
#include <map>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <atomic>


namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class Timer;
typedef std::shared_ptr<Timer> TimerPtr;
class MessageReader;
typedef std::shared_ptr<MessageReader> MessageReaderPtr;
class MessageWriter;
typedef std::shared_ptr<MessageWriter> MessageWriterPtr;

class ShardCoordinator;
class ShardEntry;
typedef std::vector<ShardEntry> ShardEntryVec;
class TopicMeta;
typedef std::shared_ptr<TopicMeta> TopicMetaPtr;
class ShardMeta;
typedef std::shared_ptr<ShardMeta> ShardMetaPtr;
class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;

class MetaData
{
public:
    MetaData(const std::string& projectName, const std::string& topicName, const std::string& subId, const CommonConfiguration& commonConf);

    void Init();
    void InitTopicMeta();
    void UpdateShardMetaMap();

    void Register(ShardCoordinator* coordinatorPtr);
    void Unregister(ShardCoordinator* coordinatorPtr);
    void UpdateShardOnceRightNow();

    TopicMetaPtr GetTopicMeta() const { return mTopicMeta; };
    const std::map<std::string, ShardMetaPtr>& GetShardMetaMap() const { return mShardMetaMap; };
    const MessageReaderPtr& GetMessageReader() const { return mMessageReader; };
    const MessageWriterPtr& GetMessageWriter() const { return mMessageWriter; };

private:
    bool CheckStateReady(const ShardState& state);
    bool OnShardChange(const std::map<std::string, ShardMetaPtr>& newShardMetaMap);

private:
    std::atomic_bool mUpdating;
    std::string mEndpoint;
    std::string mProjectName;
    std::string mTopicName;
    std::string mSubId;
    std::string mUniqKey;
    CommonConfiguration mCommonConf;
    int32_t mThreadNum;
    TimerPtr mUpdateShardTimer;
    TopicMetaPtr mTopicMeta;
    std::map<std::string, ShardMetaPtr> mShardMetaMap;
    std::set<ShardCoordinator*> mCoordinators;

    DatahubClientPtr mClientPtr;
    MessageReaderPtr mMessageReader;
    MessageWriterPtr mMessageWriter;
    log4cpp::Category* mLoggerPtr;
};

typedef std::shared_ptr<MetaData> MetaDataPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_META_DATA_H