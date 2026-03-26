#include "record_pack_queue.h"
#include "timer.h"
#include "record_pack.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"
#include "client/write_result.h"

namespace aliyun
{
namespace datahub
{

RecordPackQueue::RecordPackQueue(int64_t maxBufferSize, int64_t maxBufferRecordCount, int64_t maxBufferTimeMs, int32_t maxRecordPackQueueLimit)
    : mLastObtainTimeMs(Timer::GetCurrentTimeMs()),
      mMaxBufferSize(maxBufferSize),
      mMaxBufferRecordCount(maxBufferRecordCount),
      mMaxBufferTimeMs(maxBufferTimeMs),
      mMaxRecordPackQueueLimit(maxRecordPackQueueLimit)
{}

bool RecordPackQueue::IsReady() const
{
    return Timer::GetCurrentTimeMs() - mLastObtainTimeMs >= mMaxBufferTimeMs;
}

RecordPackPtr RecordPackQueue::ObtainReadyRecordPack(bool forceWrite)
{
    std::unique_lock<std::mutex> lock(mQueueMutex);
    if ((forceWrite || IsReady()) && mCurrentRecordPack != nullptr)
    {
        mReadyRecordPacks.push(mCurrentRecordPack);
        mCurrentRecordPack = nullptr;
        mLastObtainTimeMs = Timer::GetCurrentTimeMs();
    }

    return mReadyRecordPacks.empty() ? nullptr : mReadyRecordPacks.front();
}

void RecordPackQueue::PopBackRecordPack()
{
    std::unique_lock<std::mutex> lock(mQueueMutex);
    if (!mReadyRecordPacks.empty())
    {
        mReadyRecordPacks.pop();
        mQueueCV.notify_one();
    }
}

WriteResultFuturePtr RecordPackQueue::AppendRecord(const RecordEntryVec& records)
{
    Timer timer(1000);
    std::function<bool()> func = [this]{ return mReadyRecordPacks.size() < mMaxRecordPackQueueLimit; };
    if (!timer.WaitFor(mQueueMutex, mQueueCV, func))
    {
        return nullptr;
    }

    std::unique_lock<std::mutex> lock(mQueueMutex);
    WriteResultFuturePtr result = TryAppend(records);
    if (mCurrentRecordPack != nullptr && mCurrentRecordPack->IsReady())
    {
        mReadyRecordPacks.push(std::move(mCurrentRecordPack));
    }
    if (result == nullptr)
    {
        mCurrentRecordPack = std::make_shared<RecordPack>(mMaxBufferSize, mMaxBufferRecordCount, mMaxBufferTimeMs);
        result = mCurrentRecordPack->TryAppend(records);
    }
    return result;
}

WriteResultFuturePtr RecordPackQueue::TryAppend(const RecordEntryVec& records)
{
    if (mCurrentRecordPack != nullptr)
    {
        return mCurrentRecordPack->TryAppend(records);
    }
    else
    {
        return nullptr;
    }
}

} // namespace datahub
} // namespace aliyun
