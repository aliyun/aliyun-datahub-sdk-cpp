#include "datahub/datahub_subscription.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

std::string GetNameForSubscriptionType(const SubscriptionType& type)
{
    switch (type)
    {
        case USER:
            return "USER";
        case SYSTEM:
            return "SYSTEM";
        case TT:
            return "TT";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid subscription type");
    }
}

SubscriptionType GetSubscriptionTypeFromName(const std::string& name)
{
    if (name == "USER")
    {
        return USER;
    }
    else if (name == "SYSTEM")
    {
        return SYSTEM;
    }
    else if (name == "TT")
    {
        return TT;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported subscription type " + name);
    }
}

SubscriptionType GetSubscriptionTypeFromValue(int value)
{
    switch (value)
    {
        case USER:
            return USER;
        case SYSTEM:
            return SYSTEM;
        case TT:
            return TT;
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid subscription type");
    }
}

} // namespace datahub
} // namespace aliyun
