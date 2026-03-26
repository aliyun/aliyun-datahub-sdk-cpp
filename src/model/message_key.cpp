#include <string>
#include "datahub/datahub_record.h"


namespace aliyun
{
namespace datahub
{

MessageKey::MessageKey(const std::string& shardId, const std::string& nextCursor, const SubscriptionOffset& offset)
    : mShardId(shardId),
      mNextCursor(nextCursor),
      mOffset(offset)
{
    mIsReady.store(false);
}

void MessageKey::Ack()
{
    mIsReady.store(true);
}

bool MessageKey::IsReady()
{
    return mIsReady.load();
}

const std::string& MessageKey::GetShardId() const 
{
    return mShardId;
}

const std::string& MessageKey::GetNextCursor() const
{
    return mNextCursor;
}

const SubscriptionOffset& MessageKey::GetOffset() const
{
    return mOffset;
}

} // namespace datahub
} // namespace aliyun
