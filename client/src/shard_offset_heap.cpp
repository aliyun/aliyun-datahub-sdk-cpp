#include "shard_offset_heap.h"

namespace aliyun
{
namespace datahub
{

void Heap::HeapUp(std::size_t index)
{
    if (index <= 0 || index >= mNodes.size())
    {
        return ;
    }

    std::size_t parent = (index - 1) / 2;
    if (mNodes[index]->mTimestamp < mNodes[parent]->mTimestamp)
    {
        std::swap(mNodes[index], mNodes[parent]);
        mNodes[index]->mIndex = index;
        mNodes[parent]->mIndex = parent;
        HeapUp(parent);
    }
}

void Heap::HeapDown(std::size_t index)
{
    if (index < 0 || index > (mNodes.size() - 1) / 2)
    {
        return ;
    }

    std::size_t smallest = index;
    std::size_t left = 2 * index + 1, right = 2 * index + 2;
    if (left < mNodes.size() && mNodes[left]->mTimestamp < mNodes[smallest]->mTimestamp)
    {
        smallest = left;
    }
    if (right < mNodes.size() && mNodes[right]->mTimestamp < mNodes[smallest]->mTimestamp)
    {
        smallest = right;
    }

    if (smallest != index)
    {
        std::swap(mNodes[index], mNodes[smallest]);
        mNodes[index]->mIndex = index;
        mNodes[smallest]->mIndex = smallest;
        HeapDown(smallest);
    }
}

void Heap::Push(const OffsetInfoPtr& node)
{
    mNodes.emplace_back(node);
    node->mIndex = mNodes.size() - 1;
    HeapUp(mNodes.size() - 1);
}

void Heap::Pop()
{
    if (Empty())
    {
        return ;
    }

    std::swap(mNodes[0], mNodes[mNodes.size() - 1]);
    mNodes[0]->mIndex = 0u;
    mNodes[mNodes.size() - 1]->mIndex = -1u;
    mNodes.resize(mNodes.size() - 1);
    HeapDown(0u);
}

OffsetInfoPtr Heap::Top()
{
    if (Empty())
    {
        return nullptr;
    }

    return mNodes[0];
}

void Heap::Update(const OffsetInfoPtr& node, int64_t timestamp)
{
    bool isDown = timestamp > node->mTimestamp;
    node->mTimestamp = timestamp;
    if (isDown)
    {
        HeapDown(node->mIndex);
    }
    else
    {
        HeapUp(node->mIndex);
    }
}

}
}