#ifndef INCLUDE_DATAHUB_CLIENT_OFFSET_COORDINATOR_H
#define INCLUDE_DATAHUB_CLIENT_OFFSET_COORDINATOR_H

#include "offset_meta.h"
#include "shard_coordinator.h"
#include <map>
#include <atomic>


namespace aliyun
{
namespace datahub
{

class OffsetManager;
class ConsumerConfiguration;
class MessageKey;
typedef std::shared_ptr<MessageKey> MessageKeyPtr;

class OffsetCoordinator : public ShardCoordinator
{
public:
    OffsetCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf);
    virtual ~OffsetCoordinator();

    void OnSubSessionChanged();
    void OnSubOffline();
    void OnSubDeleted();
    void OnOffsetNotAck();

    virtual void OnShardReadEnd(const std::vector<std::string>& shardIds);
    virtual void OnOffsetReset();
    virtual void UpdateShardInfo();
    std::map<std::string, ConsumeOffsetMeta> InitAndGetOffsets(const StringVec& shardIds);
    void SendOffsetRequest(MessageKeyPtr messageKeyPtr);

protected:
    bool mSubSessionChanged;
    bool mSubOffline;
    bool mSubDeleted;
    bool mOffsetNotAck;
    std::atomic<bool> mOffsetReset;

    std::unique_ptr<OffsetManager> mOffsetManagerPtr;
};

typedef std::shared_ptr<OffsetCoordinator> OffsetCoordinatorPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_OFFSET_COORDINATOR_H
