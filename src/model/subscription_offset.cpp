#include <string>
#include "datahub/datahub_subscription_offset.h"

namespace aliyun
{
namespace datahub
{

SubscriptionOffset::SubscriptionOffset(
        int64_t timestamp,
        int64_t sequence,
        uint32_t batchIndex,
        int64_t version,
        int64_t sessionId) :
        mOffsetValue(OffsetValue(timestamp, sequence, batchIndex)),
        mVersion(version),
        mSessionId(sessionId)
{
}

SubscriptionOffset::~SubscriptionOffset()
{
}

void SubscriptionOffset::SetTimestamp(int64_t timestamp)
{
    mOffsetValue.mTimestamp = timestamp;
}

void SubscriptionOffset::SetSequence(int64_t sequence)
{
    mOffsetValue.mSequence = sequence;
}

void SubscriptionOffset::SetBatchIndex(uint32_t batchIndex)
{
    mOffsetValue.mBatchIndex = batchIndex;
}

void SubscriptionOffset::SetVersion(int64_t version)
{
    mVersion = version;
}

void SubscriptionOffset::SetSessionId(int64_t sessionId)
{
    mSessionId = sessionId;
}

int64_t SubscriptionOffset::GetTimestamp() const
{
    return mOffsetValue.mTimestamp;
}

int64_t SubscriptionOffset::GetSequence() const
{
    return mOffsetValue.mSequence;
}

uint32_t SubscriptionOffset::GetBatchIndex() const
{
    return mOffsetValue.mBatchIndex;
}

int64_t SubscriptionOffset::GetVersion() const
{
    return mVersion;
}

int64_t SubscriptionOffset::GetSessionId() const
{
    return mSessionId;
}

} // namespace datahub
} // namespace aliyun
