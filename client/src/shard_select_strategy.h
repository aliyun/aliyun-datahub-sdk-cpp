#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_SELECT_STRATEGY_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_SELECT_STRATEGY_H

#include <set>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include "shard_offset_heap.h"

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
class RecordResult;
typedef std::shared_ptr<RecordResult> RecordResultPtr;

class ShardReaderSelectStrategy
{
public:
    ShardReaderSelectStrategy();

    void AddShard(const std::string& shardId, int64_t systemTime = -1l);
    void RemoveShard(const std::string& shardId);
    std::string GetNextShard();
    void AfterRead(const std::string& shardId, RecordResultPtr recordPtr);

private:
    TimerPtr mTimer;
    std::mutex mMutex;
    log4cpp::Category* mLoggerPtr;
    std::set<std::string> mEmptyShards;
    std::unordered_map<std::string, OffsetInfoPtr> mShardOffsetMap;
    Heap mShardOffsetInfos;
};

class ShardWriterSelectStrategy
{
public:
    ShardWriterSelectStrategy();

    void AddShard(const std::string& shardId);
    void RemoveShard(const std::string& shardId);
    void ClearAllShard();
    void AddInvalidShard(const std::string& shardId);
    std::string GetNextShard();

private:
    void RemoveShardFromActive(const std::string& shardId);

private:
    TimerPtr mTimer;
    std::mutex mMutex;
    int64_t mShardIndex;
    log4cpp::Category* mLoggerPtr;
    std::vector<std::string> mActiveShards;
    std::set<std::string> mInactivateShards;
};

typedef std::shared_ptr<ShardReaderSelectStrategy> ShardReaderSelectStrategyPtr;
typedef std::shared_ptr<ShardWriterSelectStrategy> ShardWriterSelectStrategyPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_SELECT_STRATEGY_H
