#ifndef INCLUDE_DATAHUB_CLIENT_CONSUMER_COORDINATOR_H
#define INCLUDE_DATAHUB_CLIENT_CONSUMER_COORDINATOR_H

#include "offset_coordinator.h"
#include "datahub/datahub_typedef.h"
#include <string>


namespace aliyun
{
namespace datahub
{

class SyncGroupMeta;
typedef std::shared_ptr<SyncGroupMeta> SyncGroupMetaPtr;
class ConsumerHeartbeat;
typedef std::shared_ptr<ConsumerHeartbeat> ConsumerHeartbeatPtr;

class ConsumerCoordinator : public OffsetCoordinator
{
public:
    ConsumerCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf);
    virtual ~ConsumerCoordinator();

    virtual void OnShardReadEnd(const StringVec& shardIds);
    virtual void OnOffsetReset();
    virtual bool WaitShardAssign() const;
    virtual void UpdateShardInfo();
    void OnShardChange(const StringVec& addShards, const StringVec& delShards);

    std::string GetConsumerId() const { return mConsumerId; };
    int64_t GetVersionId() const { return mVersionId; };
    int64_t GetSessionTimeout() const { return mSessionTimeout; };
    SyncGroupMetaPtr GetSyncGroupMetaPtr() const { return mSyncGroupMetaPtr; };

protected:
    void JoinGroup();
    void StartHeartBeat();
    void LeaveGroup();
    void StopHeartBeat();
    void JoinGroupAndStartHeartBeat();
    void LeaveGroupAndStopHeartBeat();
    void SyncConsuemrGroup();

protected:
    std::string mConsumerId;
    int64_t mVersionId;
    int64_t mSessionTimeout;

    SyncGroupMetaPtr mSyncGroupMetaPtr;
    ConsumerHeartbeatPtr mHeartbeatPtr;
};

typedef std::shared_ptr<ConsumerCoordinator> ConsumerCoordinatorPtr;

} // namespace datahub
} // namespace aliyun

#endif  // INCLUDE_DATAHUB_CLIENT_CONSUMER_COORDINATOR_H