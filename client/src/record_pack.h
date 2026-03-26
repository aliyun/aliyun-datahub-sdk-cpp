#ifndef INCLUDE_DATAHUB_CLIENT_RECORD_PACK_H
#define INCLUDE_DATAHUB_CLIENT_RECORD_PACK_H

#include <vector>
#include <memory>
#include <future>
#include <mutex>

namespace aliyun
{
namespace datahub
{

class WriteResult;
typedef std::shared_ptr<WriteResult> WriteResultPtr;
typedef std::shared_future<WriteResultPtr> WriteResultFuture;
typedef std::shared_ptr<WriteResultFuture> WriteResultFuturePtr;
typedef std::shared_ptr<std::promise<WriteResultPtr> > WriteResultPromisePtr;
class RecordEntry;
typedef std::vector<RecordEntry> RecordEntryVec;

class RecordPack
{
public:
    RecordPack(int64_t maxBufferSize, int64_t maxBufferRecordCount, int64_t maxBufferTimeMs);

    WriteResultFuturePtr TryAppend(const RecordEntryVec& records);
    bool IsReady() const;

    int64_t GetInitTimeMs() const { return mInitTimeMs; }
    int64_t GetCurrSize() const { return mCurrSize; }
    int64_t GetCurrCount() const { return mCurrCount; }
    const RecordEntryVec& GetRecords() const { return mRecords; }
    const std::vector<WriteResultPromisePtr>& GetWriteResults() const { return mWriteResults; }

private:
    WriteResultFuturePtr AppendRecords(const RecordEntryVec& records, int64_t size);
    int64_t GetTotalRecordSize(const RecordEntryVec& records);

private:
    bool mIsReady;
    std::mutex mMutex;
    int64_t mInitTimeMs;
    int64_t mCurrSize;
    int64_t mCurrCount;
    int64_t mMaxBufferSize;
    int64_t mMaxBufferRecordCount;
    int64_t mMaxBufferTimeMs;

    RecordEntryVec mRecords;
    std::vector<WriteResultPromisePtr> mWriteResults;
};

typedef std::shared_ptr<RecordPack> RecordPackPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_RECORD_PACK_H