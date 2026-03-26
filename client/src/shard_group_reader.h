#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_GROUP_READER_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_GROUP_READER_H

#include "offset_meta.h"
#include "datahub/datahub_typedef.h"
#include "client/consumer/consumer_config.h"
#include <map>
#include <mutex>
#include <string>


namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class ShardReaderSelectStrategy;
typedef std::shared_ptr<ShardReaderSelectStrategy> ShardReaderSelectStrategyPtr;
class OffsetCoordinator;
typedef std::shared_ptr<OffsetCoordinator> OffsetCoordinatorPtr;
class ShardReader;
typedef std::shared_ptr<ShardReader> ShardReaderPtr;
class RecordResult;
typedef std::shared_ptr<RecordResult> RecordResultPtr;
class MessageKey;
typedef std::shared_ptr<MessageKey> MessageKeyPtr;

class ShardGroupReader
{
public:
    ShardGroupReader(OffsetCoordinator* coordinatorPtr, const ConsumerConfiguration& consumerConf, const StringVec& shardIds, int64_t timestamp);
    ~ShardGroupReader();

    RecordResultPtr Read(const std::string& shardId, int64_t timeout);
    void OnShardChange(const StringVec& addShards, const StringVec& delShards);
    void OnRemoveAllShards();

private:
    std::map<std::string, ConsumeOffsetMeta> GenShardOffset(const StringVec& shardIds, int64_t timestamp);
    ShardReaderPtr GetNextReader(const std::string& shardId);
    ShardReaderPtr GetShardReader(const std::string& shardId);

    void CreateShardReader(const StringVec& shardIds, int64_t timestamp);
    void RemoveShardReader(const StringVec& shardIds);
    void RemoveAllShardReader();

    void AckLastMessageKey();
    void CheckTimeout(int64_t& timeout);
    RecordResultPtr ReadOnceByReader(ShardReaderPtr readerPtr);

public:
    static thread_local MessageKeyPtr sLastMessagekeyPtr;

private:
    bool mClosed;
    bool mAutoAckOffset;
    int32_t mFetchNum;
    std::mutex mMutex;
    std::mutex mShardReaderMutex;
    ConsumerConfiguration mConsumerConf;

    std::map<std::string, ShardReaderPtr> mShardReaderMap;

    log4cpp::Category* mLoggerPtr;
    ShardReaderSelectStrategyPtr mShardSelector;
    OffsetCoordinator* mCoordinatorPtr;
};

typedef std::shared_ptr<ShardGroupReader> ShardGroupReaderPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_GROUP_READER_H