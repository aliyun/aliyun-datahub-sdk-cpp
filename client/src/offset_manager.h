#ifndef INCLUDE_DATAHUB_CLIENT_OFFSET_MANAGER_H
#define INCLUDE_DATAHUB_CLIENT_OFFSET_MANAGER_H

#include "offset_meta.h"
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_subscription_offset.h"
#include <map>
#include <queue>
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

class Timer;
class Configuration;
class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;
class MessageKey;
typedef std::shared_ptr<MessageKey> MessageKeyPtr;

class OffsetManager
{
public:
    OffsetManager(const std::string& projectName, const std::string& topicName, const std::string& subId, const Configuration& conf);
    ~OffsetManager();

    void UpdateOffsetMeta(const std::string& shardId, int64_t version, int64_t sessionId);
    void SendOffsetRequest(MessageKeyPtr messageKeyPtr);
    void OnShardRealese(const StringVec& shardIds);
    void OnOffsetReset();

private:
    void CommitOffsetTask();
    void SyncOffset();
    void CommitOffset();
    int64_t GetMinTimestamp();
    void CommitOnceRightNow();
    void ForceCommitOffset(const StringVec& shardIds);
    bool CheckRequestQueueIsEmpty(const StringVec& shardIds);
    void Start();


private:
    bool mClosed;
    std::unique_ptr<Timer> mTimer;
    std::string mProjectName;
    std::string mTopicName;
    std::string mSubId;
    std::string mUniqKey;
    std::mutex mMutex;
    std::thread mCommitThread;
    std::map<std::string, OffsetMeta> mOffsetMetaMap;
    std::map<std::string, std::queue<MessageKeyPtr> > mOffsetRequestMap;
    std::map<std::string, SubscriptionOffset> mLastOffsetMap;

    log4cpp::Category* mLoggerPtr;
    DatahubClientPtr mClientPtr;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_OFFSET_MANAGER_H