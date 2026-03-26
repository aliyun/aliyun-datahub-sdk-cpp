#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_META_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_META_H

#include "datahub/datahub_shard.h"
#include <string>


namespace aliyun
{
namespace datahub
{

class ShardMeta
{
public:
    ShardMeta(const std::string& shardId, const ShardState& shardState)
        : mShardId(shardId),
          mShardState(shardState)
    {}

    std::string GetShardId() const { return mShardId; }
    const ShardState& GetShardState() const { return mShardState; }

private:
    std::string mShardId;
    ShardState mShardState;
};

typedef std::shared_ptr<ShardMeta> ShardMetaPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_META_H
