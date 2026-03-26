#ifndef INCLUDE_DATAHUB_CLIENT_CONSUMER_HEARTBEAT_H
#define INCLUDE_DATAHUB_CLIENT_CONSUMER_HEARTBEAT_H

#include "datahub/datahub_result.h"
#include <mutex>
#include <string>
#include <thread>


namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class Configuration;
class Timer;
class SyncGroupMeta;
class ConsumerCoordinator;
class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;

class ConsumerHeartbeat
{
public:
    ConsumerHeartbeat(const std::string& projectName, const std::string& topicName, const std::string& subId,
        int64_t sessionTimeout,  const Configuration& conf, ConsumerCoordinator* coordinatorPtr);
    ~ConsumerHeartbeat();
    bool NeedRejoin() const;
    bool WaitingShardAssign() const;

private:
    void Start();
    void KeepHeartbeat();
    void HeartbeatOnce();
    bool HasValidShards() const;
    void OnShardChange(const StringVec& oldShards, const StringVec& newShards);

private:
    bool mClosed;
    bool mConsumerNotFound;

    std::string mProjectName;
    std::string mTopicName;
    std::string mSubId;
    std::string mUniqKey;
    std::string mConsumerId;
    int64_t mVersionId;
    int64_t mSessionTimeout;
    int64_t mHeartbeatTimeout;
    StringVec mCurrShards;

    std::unique_ptr<Timer> mTimer;
    std::mutex mMutex;
    HeartbeatResult mHeartbeatResult;
    std::thread mHeartbeatThread;

    log4cpp::Category* mLoggerPtr;
    DatahubClientPtr mClientPtr;
    ConsumerCoordinator* mCoordinatorPtr;
};

typedef std::shared_ptr<ConsumerHeartbeat> ConsumerHeartbeatPtr;

} // namespace datahub
} // namespace aliyun

#endif  // INCLUDE_DATAHUB_CLIENT_CONSUMER_HEARTBEAT_H