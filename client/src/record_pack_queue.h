#ifndef INCLUDE_DATAHUB_CLIENT_RECORD_PACK_QUEUE_H
#define INCLUDE_DATAHUB_CLIENT_RECORD_PACK_QUEUE_H

#include <stdint.h>
#include <queue>
#include <memory>
#include <future>
#include <mutex>

namespace aliyun
{
namespace datahub
{

class RecordEntry;
typedef std::vector<RecordEntry> RecordEntryVec;
class WriteResult;
typedef std::shared_ptr<WriteResult> WriteResultPtr;
typedef std::shared_future<WriteResultPtr> WriteResultFuture;
typedef std::shared_ptr<WriteResultFuture> WriteResultFuturePtr;
class RecordPack;
typedef std::shared_ptr<RecordPack> RecordPackPtr;

class RecordPackQueue
{
public:
    RecordPackQueue(int64_t maxBufferSize, int64_t maxBufferRecordCount, int64_t maxBufferTimeMs, int32_t maxRecordPackQueueLimit);

    WriteResultFuturePtr AppendRecord(const RecordEntryVec& records);
    RecordPackPtr ObtainReadyRecordPack(bool forceWrite);
    void PopBackRecordPack();

private:
    bool IsReady() const;
    WriteResultFuturePtr TryAppend(const RecordEntryVec& records);

private:
    std::mutex mQueueMutex;
    std::condition_variable mQueueCV;
    int64_t mLastObtainTimeMs;
    int64_t mMaxBufferSize;
    int64_t mMaxBufferRecordCount;
    int64_t mMaxBufferTimeMs;
    uint32_t mMaxRecordPackQueueLimit;
    std::queue<RecordPackPtr> mReadyRecordPacks;
    RecordPackPtr mCurrentRecordPack;
};

typedef std::shared_ptr<RecordPackQueue> RecordPackQueuePtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_RECORD_PACK_QUEUE_H