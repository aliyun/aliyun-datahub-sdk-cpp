#ifndef INCLUDE_DATAHUB_CLIENT_SHARD_OFFSET_HEAP_H
#define INCLUDE_DATAHUB_CLIENT_SHARD_OFFSET_HEAP_H

#include <vector>
#include <memory>

namespace aliyun
{
namespace datahub
{

struct OffsetInfo
{
    bool mValid;
    uint32_t mIndex;
    std::string mShardId;
    int64_t mTimestamp;

    OffsetInfo(const std::string& shard, int64_t timestamp)
        : OffsetInfo(true, -1u, shard, timestamp) {}

    OffsetInfo(bool valid, uint32_t index, const std::string& shard, int64_t timestamp)
        : mValid(valid)
        , mIndex(index)
        , mShardId(shard)
        , mTimestamp(timestamp) {}
};
typedef std::shared_ptr<OffsetInfo> OffsetInfoPtr;

class Heap
{
public:
    void Pop();
    OffsetInfoPtr Top();
    void Push(const OffsetInfoPtr& node);
    void Update(const OffsetInfoPtr& node, int64_t timestamp);

    bool Empty() const { return mNodes.empty(); }

private:
    void HeapUp(std::size_t index);
    void HeapDown(std::size_t index);

private:
    std::vector<OffsetInfoPtr> mNodes;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_SHARD_OFFSET_HEAP_H