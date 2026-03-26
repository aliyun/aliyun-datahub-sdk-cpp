#include <string>
#include "datahub/datahub_shard.h"

namespace aliyun
{
namespace datahub
{

ShardEntry::ShardEntry() :
        mState(ShardState::OPENING),
        mClosedTime(0)
{
}

ShardEntry::~ShardEntry()
{
}

std::string ShardEntry::GetShardId() const
{
    return mShardId;
}

ShardState ShardEntry::GetState() const
{
    return mState;
}

std::string ShardEntry::GetBeginHashKey() const
{
    return mBeginHashKey;
}

std::string ShardEntry::GetEndHashKey() const
{
    return mEndHashKey;
}

std::string ShardEntry::GetRightShardId() const
{
    return mRightShardId;
}

std::string ShardEntry::GetLeftShardId() const
{
    return mLeftShardId;
}

const StringVec& ShardEntry::GetParentShardIds() const
{
    return mParentShardIds;
}

int64_t ShardEntry::GetClosedTime() const
{
    return mClosedTime;
}

} // namespace datahub
} // namespace aliyun
