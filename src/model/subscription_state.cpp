#include "datahub/datahub_subscription.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

std::string GetNameForSubscriptionState(const SubscriptionState& state)
{
    switch (state)
    {
        case OFFLINE:
            return "OFFLINE";
        case ONLINE:
            return "ONLINE";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid subscription state");
    }
}

SubscriptionState GetSubscriptionStateFromName(const std::string& name)
{
    if (name == "OFFLINE")
    {
        return OFFLINE;
    }
    else if (name == "ONLINE")
    {
        return ONLINE;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported subscription state " + name);
    }
}

SubscriptionState GetSubscriptionStateFromValue(int value)
{
    switch (value)
    {
        case OFFLINE:
            return OFFLINE;
        case ONLINE:
            return ONLINE;
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid subscription state");
    }
}

} // namespace datahub
} // namespace aliyun
