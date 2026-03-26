#ifndef DATAHUB_DATAHUB_SUBSCRIPTION_H
#define DATAHUB_DATAHUB_SUBSCRIPTION_H

#include <string>

namespace aliyun
{
namespace datahub
{

enum SubscriptionType {
    USER = 0,
    SYSTEM = 1,
    TT = 2
};

std::string GetNameForSubscriptionType(const SubscriptionType& type);

SubscriptionType GetSubscriptionTypeFromName(const std::string& name);

SubscriptionType GetSubscriptionTypeFromValue(int value);

enum SubscriptionState {
    OFFLINE = 0,
    ONLINE = 1
};

std::string GetNameForSubscriptionState(const SubscriptionState& state);

SubscriptionState GetSubscriptionStateFromName(const std::string& name);

SubscriptionState GetSubscriptionStateFromValue(int value);

class SubscriptionEntry
{
public:
    SubscriptionEntry();
    ~SubscriptionEntry();

    std::string GetSubId() const;
    std::string GetComment() const;
    bool GetIsOwner() const;
    SubscriptionType GetType() const;
    SubscriptionState GetState() const;
    int64_t GetCreateTime() const;
    int64_t GetLastModifyTime() const;

private:
    friend class JsonTool;
    std::string mSubId;
    std::string mComment;
    bool mIsOwner;
    SubscriptionType mType;
    SubscriptionState mState;
    int64_t mCreateTime;
    int64_t mLastModifyTime;
};

} // namespace datahub
} // namespace aliyun

#endif //DATAHUB_DATAHUB_SUBSCRIPTION_H
