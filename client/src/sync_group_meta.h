#ifndef INCLUDE_DATAHUB_CLIENT_SYNC_GROUP_META_H
#define INCLUDE_DATAHUB_CLIENT_SYNC_GROUP_META_H

#include "datahub/datahub_typedef.h"
#include <mutex>

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

class SyncGroupMeta
{
public:
    SyncGroupMeta();

    void OnShardRelease(const StringVec& shardIds);
    void OnShardReadEnd(const StringVec& shardIds);
    void ClearShardRelease();
    void OnHeartBeatDone(const StringVec& shardIds);
    void OnSyncDone();
    bool NeedSyncGroup();
    StringVec ObtainValidShards();
    const StringVec& GetReleaseShards() const { return mReleaseShards; }
    const StringVec& GetReadEndShards() const { return mReadEndShards; }

private:
    void AddAll(const StringVec& src, StringVec& tar);
    void RemoveAll(const StringVec& src, StringVec& tar);

private:
    TimerPtr mTimer;
    std::mutex mMutex;
    StringVec mReleaseShards;
    StringVec mReadEndShards;
    StringVec mActiveShards;
    log4cpp::Category* mLoggerPtr;
};

typedef std::shared_ptr<SyncGroupMeta> SyncGroupMetaPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SYNC_GROUP_META_H
