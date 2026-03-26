#include "datahub/datahub_shard.h"

namespace aliyun
{
namespace datahub
{

std::string GetNameForShardState(const ShardState& shardState)
{
    switch (shardState)
    {
        case OPENING:
            return "OPENING";
        case ACTIVE:
            return "ACTIVE";
        case CLOSED:
            return "CLOSED";
        case CLOSING:
            return "CLOSING";
        default:
            return "OPENING";
    }
}

ShardState GetShardStateForName(const std::string& name)
{
    if (name == "OPENING")
    {
        return OPENING;
    }
    else if (name == "ACTIVE")
    {
        return ACTIVE;
    }
    else if (name == "CLOSED")
    {
        return CLOSED;
    }
    else if (name == "CLOSING")
    {
        return CLOSING;
    }
    else
    {
        // XXX:
        return OPENING;
    }
}

} // namespace datahub
} // namespace aliyun
