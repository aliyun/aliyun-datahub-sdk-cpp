#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_WRITER_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_WRITER_H

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <future>
#include <mutex>


namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class ProducerConfiguration;
class RecordEntry;
typedef std::vector<RecordEntry> RecordEntryVec;
class MessageWriter;
typedef std::shared_ptr<MessageWriter> MessageWriterPtr;
class ShardWriterSelectStrategy;
typedef std::shared_ptr<ShardWriterSelectStrategy> ShardWriterSelectStrategyPtr;
class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;
class WriteResult;
typedef std::shared_ptr<WriteResult> WriteResultPtr;
typedef std::shared_future<WriteResultPtr> WriteResultFuture;
typedef std::shared_ptr<WriteResultFuture> WriteResultFuturePtr;
class RecordPack;
typedef std::shared_ptr<RecordPack> RecordPackPtr;
class RecordPackQueue;
typedef std::shared_ptr<RecordPackQueue> RecordPackQueuePtr;

class ShardWriter
{
public:
    ShardWriter(const std::string& projectName, const std::string& topicName, const ProducerConfiguration& producerConf, const std::string& shardId,
            const MessageWriterPtr messageWriter, const ShardWriterSelectStrategyPtr& shardSelector);
    ~ShardWriter();

    void Write(const RecordEntryVec& records);
    WriteResultFuturePtr WriteAsync(const RecordEntryVec& records);

    void WriteOnce(const RecordEntryVec& records);

    void Flush();

    std::string GetShardId() const { return mShardId; }
    std::mutex* GetQueueMutex() const { return mQueueMutex.get(); }
    RecordPackQueuePtr GetRecordPackQueue() const { return mRecordPackQueuePtr; }

private:
    void SendNextTask(bool forceFlush);
    void SendTaskOnce(const RecordPackPtr& recordPack);
    void TaskDone();

private:
    bool mClosed;
    std::string mProjectName;
    std::string mTopicName;
    std::string mSubId;
    std::string mUniqKey;
    std::string mShardId;
    int32_t mRetryTimes;

    std::atomic<int64_t> mTotalTaskNum;
    std::mutex mTaskMutex;
    std::condition_variable mTaskCV;

    std::mutex mSendMutex;
    std::atomic<int64_t> mWriteTotalNum;
    std::unique_ptr<std::mutex> mQueueMutex;
    RecordPackQueuePtr mRecordPackQueuePtr;

    log4cpp::Category* mLoggerPtr;
    DatahubClientPtr mClientPtr;
    MessageWriterPtr mMessageWriter;
    ShardWriterSelectStrategyPtr mShardSelector;
};

typedef std::shared_ptr<ShardWriter> ShardWriterPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_WRITER_H