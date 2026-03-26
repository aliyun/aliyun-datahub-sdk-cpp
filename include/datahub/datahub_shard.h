#ifndef DATAHUB_SDK_DATAHUB_SHARD_H
#define DATAHUB_SDK_DATAHUB_SHARD_H

#include <string>
#include <vector>
#include <cstdint>
#include "datahub/datahub_typedef.h"

namespace aliyun
{
namespace datahub
{

enum ShardState
{
    OPENING = 0,
    ACTIVE = 1,
    CLOSED = 2,
    CLOSING = 3
};

std::string GetNameForShardState(const ShardState& shardState);

ShardState GetShardStateForName(const std::string& name);

class ShardDesc
{
public:
    ShardDesc();
    ~ShardDesc();

    std::string GetShardId() const;
    std::string GetBeginHashKey() const;
    std::string GetEndHashKey() const;

private:
    friend class JsonTool;
    std::string mShardId;
    std::string mBeginHashKey;
    std::string mEndHashKey;
};

class ShardEntry
{
public:
    ShardEntry();
    ~ShardEntry();

    std::string GetShardId() const;
    ShardState GetState() const;
    std::string GetBeginHashKey() const;
    std::string GetEndHashKey() const;
    std::string GetRightShardId() const;
    std::string GetLeftShardId() const;
    const StringVec& GetParentShardIds() const;
    int64_t GetClosedTime() const;

private:
    friend class JsonTool;
    std::string mShardId;
    ShardState mState;
    std::string mBeginHashKey;
    std::string mEndHashKey;
    std::string mRightShardId;
    std::string mLeftShardId;
    StringVec mParentShardIds;
    int64_t mClosedTime;
};

} // namespace datahub
} // namespace aliyun
#endif
