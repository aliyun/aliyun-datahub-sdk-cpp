#include <string>
#include "datahub/datahub_subscription.h"

namespace aliyun
{
namespace datahub
{

SubscriptionEntry::SubscriptionEntry() :
        mIsOwner(false),
        mType(SubscriptionType::USER),
        mState(SubscriptionState::OFFLINE),
        mCreateTime(0),
        mLastModifyTime(0)
{
}

SubscriptionEntry::~SubscriptionEntry()
{
}

std::string SubscriptionEntry::GetSubId() const
{
    return mSubId;
}

std::string SubscriptionEntry::GetComment() const
{
    return mComment;
}

bool SubscriptionEntry::GetIsOwner() const
{
    return mIsOwner;
}

SubscriptionType SubscriptionEntry::GetType() const
{
    return mType;
}

SubscriptionState SubscriptionEntry::GetState() const
{
    return mState;
}

int64_t SubscriptionEntry::GetCreateTime() const
{
    return mCreateTime;
}

int64_t SubscriptionEntry::GetLastModifyTime() const
{
    return mLastModifyTime;
}

} // namespace datahub
} // namespace aliyun
