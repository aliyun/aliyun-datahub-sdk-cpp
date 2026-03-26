#ifndef INCLUDE_DATAHUB_CLIENT_SAHRD_READER_H
#define INCLUDE_DATAHUB_CLIENT_SAHRD_READER_H

#include "offset_meta.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_request.h"
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>


namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

enum CompleteType
{
    T_NORMAL,
    T_EXCEPTION,
    T_DELAY
};

class Timer;
typedef std::shared_ptr<Timer> TimerPtr;

struct CompletedFetch
{
    CompletedFetch(const std::string& shardId, const RecordResult& record) : mShardId(shardId)
    {
        mType = T_NORMAL;
        mRecordPtr = std::make_shared<RecordResult>(record);
        mExceptionPtr = nullptr;
        mTimerPtr = nullptr;
    }

    CompletedFetch(const std::string& shardId, const std::exception_ptr& ePtr) : mShardId(shardId)
    {
        mType = T_EXCEPTION;
        mExceptionPtr = ePtr;
        mRecordPtr = nullptr;
        mTimerPtr = nullptr;
    }

    CompletedFetch(const std::string& shardId, int64_t timeout) : mShardId(shardId)
    {
        mType = T_DELAY;
        mTimerPtr = std::make_shared<Timer>(timeout);
        mRecordPtr = nullptr;
        mExceptionPtr = nullptr;
    }

    CompleteType mType;
    std::string mShardId;
    RecordResultPtr mRecordPtr;
    std::exception_ptr mExceptionPtr;
    TimerPtr mTimerPtr;
};

class ConsumerConfiguration;
class MessageReader;
typedef std::shared_ptr<MessageReader> MessageReaderPtr;
class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;

class ShardReader
{
public:
    ShardReader(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf,
        const MessageReaderPtr& messageReaderPtr, const std::string& shardId, const ConsumeOffsetMeta& offset, int32_t fetchNum = 1000);
    ~ShardReader();
 
    RecordResultPtr Read(int64_t timeout);
    void ResetOffset();
    std::string GetShardId() const { return mShardId; };

private:
    void FetchOnce();
    void SendTaskAndWait(Timer& timer);
    void SendNextTask();
    void NotifyAllFetchWait();
    RecordResultPtr ReadNext(int64_t timeout);
    std::string GetNextCursor() const;
    std::string ObtainCursor(const CursorType& type, int64_t value) const;
    GetRecordResult GetNextRecordResult(const std::string& cursor);

private:
    bool mClosed;
    int32_t mFetchNum;
    uint32_t mRecordFetchQueueLimit;
    std::string mProjectName;
    std::string mTopicName;
    std::string mSubId;
    std::string mUniqKey;
    std::string mShardId;
    ConsumeOffsetMeta mCurrOffset;

    std::condition_variable mFetchCV;
    std::mutex mFetchMutex;                     // 保证CacheQueue的线程安全;
    std::mutex mReadMutex;                      // 保证每个时刻只有一个线程在读数据， 包含整个this->Read();
    std::queue<CompletedFetch> mCompletedFetchCacheQueue;
    std::atomic<int64_t> mFetchTotalNum;        // 记录从当前shard消费的数据总量
    std::atomic<bool> mTaskRuning;
    TimerPtr mLastTimer;

    log4cpp::Category* mLoggerPtr;
    MessageReaderPtr mMessageReaderPtr;
    DatahubClientPtr mClientPtr;
};

typedef std::shared_ptr<ShardReader> ShardReaderPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_READER_H