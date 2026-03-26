#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_GROUP_WRITER_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_GROUP_WRITER_H

#include <map>
#include <mutex>
#include <atomic>
#include <future>
#include <memory>
#include <vector>
#include <thread>
#include "client/producer/producer_config.h"
#include "datahub/datahub_typedef.h"

namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class MessageWriter;
typedef std::shared_ptr<MessageWriter> MessageWriterPtr;
class ShardCoordinator;
class ShardWriter;
typedef std::shared_ptr<ShardWriter> ShardWriterPtr;
class ShardWriterSelectStrategy;
typedef std::shared_ptr<ShardWriterSelectStrategy> ShardWriterSelectStrategyPtr;
class RecordEntry;
typedef std::vector<RecordEntry> RecordEntryVec;
class WriteResult;
typedef std::shared_ptr<WriteResult> WriteResultPtr;
typedef std::shared_future<WriteResultPtr> WriteResultFuture;
typedef std::shared_ptr<WriteResultFuture> WriteResultFuturePtr;
class RecordPack;
typedef std::shared_ptr<RecordPack> RecordPackPtr;

class ShardGroupWriter
{
public:
    ShardGroupWriter(ShardCoordinator* coordinator, const StringVec& shardIds, const ProducerConfiguration& producerConf);
    ~ShardGroupWriter();

    std::string Write(const RecordEntryVec& records);
    WriteResultFuturePtr WriteAsync(const RecordEntryVec& records);
    void Flush();       // Only for WriteAsync

    void OnShardChange(const StringVec& addShards, const StringVec& delShards);
    void OnRemoveAllShards();

private:
    ShardWriterPtr GetNextWriter();
    ShardWriterPtr GetShardWriter(const std::string& shardId);

    void CreateShardWriterWhenInit(const StringVec& shardIds);
    void CreateShardWriter(const StringVec& shardIds);
    void RemoveShardWriter(const StringVec& shardIds);
    void RemoveAllShardWriter();

    // 当前版本暂不支持指定ShardId,PatitionKey,HashKey形式的RecordEntry
    void CheckRecords(const RecordEntryVec& records);

private:
    bool mClosed;
    ProducerConfiguration mProducerConf;
    std::mutex mShardWriterMutex;

    ShardWriterSelectStrategyPtr mShardSelector;
    std::map<std::string, ShardWriterPtr> mShardWriterMap;

    log4cpp::Category* mLoggerPtr;
    ShardCoordinator* mCoordinatorPtr;
    MessageWriterPtr mMessageWriterPtr;
};

typedef std::shared_ptr<ShardGroupWriter> ShardGroupWriterPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_GROUP_WRITER_H
