#include <string>
#include "datahub/datahub_shard.h"

namespace aliyun
{
namespace datahub
{

ShardDesc::ShardDesc()
{
}

ShardDesc::~ShardDesc()
{
}

std::string ShardDesc::GetShardId() const
{
    return mShardId;
}

std::string ShardDesc::GetBeginHashKey() const
{
    return mBeginHashKey;
}

std::string ShardDesc::GetEndHashKey() const
{
    return mEndHashKey;
}

} // namespace datahub
} // namespace aliyun
