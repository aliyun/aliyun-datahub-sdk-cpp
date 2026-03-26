#include "record_pack.h"
#include "timer.h"
#include "datahub/datahub_record.h"
#include "client/write_result.h"

namespace aliyun
{
namespace datahub
{

RecordPack::RecordPack(int64_t maxBufferSize, int64_t maxBufferRecordCount, int64_t maxBufferTimeMs)
    : mIsReady(false),
      mInitTimeMs(Timer::GetCurrentTimeMs()),
      mCurrSize(0l),
      mCurrCount(0l),
      mMaxBufferSize(maxBufferSize),
      mMaxBufferRecordCount(maxBufferRecordCount),
      mMaxBufferTimeMs(maxBufferTimeMs)
{}

WriteResultFuturePtr RecordPack::TryAppend(const RecordEntryVec& records)
{
    int64_t size = GetTotalRecordSize(records);
    // 需要继续添加数据的情况:
    // 1. recordsSize + size <= MaxSize && recordsCount + count <= MaxCount 均未超出限制;
    // 2. Size 或 Count 有一个超出限制,但此时records为空(对应用户提交的单个records就已经超出限制的情况);
    if ((mCurrSize + size <= mMaxBufferSize && mCurrCount + (int64_t)records.size() <= mMaxBufferRecordCount) || mCurrSize == 0)
    {
        return AppendRecords(records, size);
    }
    else
    {
        mIsReady = true;
        return nullptr;
    }
}

bool RecordPack::IsReady() const
{
    return mIsReady || Timer::GetCurrentTimeMs() - mInitTimeMs >= mMaxBufferTimeMs;
}

WriteResultFuturePtr RecordPack::AppendRecords(const RecordEntryVec& records, int64_t size)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mRecords.insert(mRecords.end(), records.begin(), records.end());
    mCurrCount += records.size();
    mCurrSize += size;
    WriteResultPromisePtr writePromisePtr = std::make_shared<std::promise<WriteResultPtr> >();
    mWriteResults.push_back(writePromisePtr);
    return std::make_shared<WriteResultFuture>(writePromisePtr->get_future());
}

int64_t RecordPack::GetTotalRecordSize(const RecordEntryVec& records)
{
    int64_t size = 0;
    for (auto it = records.begin(); it != records.end(); it++)
    {
        size += it->GetTotalSize();
    }
    return size;
}

} // namespace datahub
} // namespace aliyun
