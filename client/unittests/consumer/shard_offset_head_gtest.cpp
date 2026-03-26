#define private public
#include "shard_offset_heap.h"
#undef private
#include "gtest/gtest.h"
#include <unordered_map>

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class ShardOffsetHeadGTest : public testing::Test
{
protected:
    virtual void SetUp()
    {
    }

    static Heap sMinHeap;

    inline void CheckHeapVec(const std::vector<std::pair<std::string, int64_t> >& expect)
    {
        ASSERT_EQ(expect.size(), sMinHeap.mNodes.size());
        for (std::size_t i = 0; i < sMinHeap.mNodes.size(); i++)
        {
            ASSERT_EQ(sMinHeap.mNodes[i]->mShardId, expect[i].first);
            ASSERT_EQ(sMinHeap.mNodes[i]->mTimestamp, expect[i].second);
        }
    }
};

Heap ShardOffsetHeadGTest::sMinHeap;

TEST_F(ShardOffsetHeadGTest, PushAndPopNodeTest)
{
    std::vector<OffsetInfoPtr> offsets;
    offsets.emplace_back(new OffsetInfo("7", 700));
    offsets.emplace_back(new OffsetInfo("5", 500));
    offsets.emplace_back(new OffsetInfo("4", 400));
    offsets.emplace_back(new OffsetInfo("1", 100));
    offsets.emplace_back(new OffsetInfo("3", 300));
    offsets.emplace_back(new OffsetInfo("6", 600));
    offsets.emplace_back(new OffsetInfo("8", 800));
    offsets.emplace_back(new OffsetInfo("2", 200));

    std::unordered_map<std::string, OffsetInfoPtr> offsetMap;
    for (const OffsetInfoPtr& offset : offsets)
    {
        offsetMap[offset->mShardId] = offset;
        sMinHeap.Push(offset);
    }

    /*
                    1
                /       \
            2               5
          /    \        /       \
        3       4       6       8
      /  
    7
    */

    std::vector<std::pair<std::string, int64_t> > expect {
        { "1", 100l },
        { "2", 200l },
        { "5", 500l },
        { "3", 300l },
        { "4", 400l },
        { "6", 600l },
        { "8", 800l },
        { "7", 700l },
    };

    CheckHeapVec(expect);

    int64_t shardId = 1l;
    while (!sMinHeap.Empty()) {
        const OffsetInfoPtr& info = sMinHeap.Top();
        ASSERT_TRUE(info->mValid);
        ASSERT_EQ(info->mIndex, 0u);
        ASSERT_EQ(info->mShardId, std::to_string(shardId));
        ASSERT_EQ(info->mTimestamp, shardId * 100l);
        shardId ++;
        sMinHeap.Pop();
    }
}

TEST_F(ShardOffsetHeadGTest, UpdateNodeTest)
{
    std::vector<OffsetInfoPtr> offsets;
    offsets.emplace_back(new OffsetInfo("7", 700));
    offsets.emplace_back(new OffsetInfo("5", 500));
    offsets.emplace_back(new OffsetInfo("4", 400));
    offsets.emplace_back(new OffsetInfo("1", 100));
    offsets.emplace_back(new OffsetInfo("3", 300));
    offsets.emplace_back(new OffsetInfo("6", 600));
    offsets.emplace_back(new OffsetInfo("8", 800));
    offsets.emplace_back(new OffsetInfo("2", 200));

    std::unordered_map<std::string, OffsetInfoPtr> offsetMap;
    for (const OffsetInfoPtr& offset : offsets)
    {
        offsetMap[offset->mShardId] = offset;
        sMinHeap.Push(offset);
    }

    sMinHeap.Update(offsetMap["2"], 900l);
    std::vector<std::pair<std::string, int64_t> > expect1 {
        { "1", 100l },
        { "3", 300l },
        { "5", 500l },
        { "7", 700l },
        { "4", 400l },
        { "6", 600l },
        { "8", 800l },
        { "2", 900l },
    };
    CheckHeapVec(expect1);

    sMinHeap.Update(offsetMap["5"], 1000l);
    std::vector<std::pair<std::string, int64_t> > expect2 {
        { "1", 100l },
        { "3", 300l },
        { "6", 600l },
        { "7", 700l },
        { "4", 400l },
        { "5", 1000l },
        { "8", 800l },
        { "2", 900l },
    };
    CheckHeapVec(expect2);

    sMinHeap.Update(offsetMap["1"], 1100l);
    std::vector<std::pair<std::string, int64_t> > expect3 {
        { "3", 300l },
        { "4", 400l },
        { "6", 600l },
        { "7", 700l },
        { "1", 1100l },
        { "5", 1000l },
        { "8", 800l },
        { "2", 900l },
    };
    CheckHeapVec(expect3);

    int64_t oldTimestamp = -1l;
    while (!sMinHeap.Empty()) {
        const OffsetInfoPtr& info = sMinHeap.Top();
        ASSERT_TRUE(oldTimestamp <= info->mTimestamp);
        ASSERT_TRUE(info->mValid);
        oldTimestamp = info->mTimestamp;
        sMinHeap.Pop();
    }
}

TEST_F(ShardOffsetHeadGTest, InvalidNodeTest)
{
    std::vector<OffsetInfoPtr> offsets;
    offsets.emplace_back(new OffsetInfo("7", 700));
    offsets.emplace_back(new OffsetInfo("5", 500));
    offsets.emplace_back(new OffsetInfo("4", 400));
    offsets.emplace_back(new OffsetInfo("1", 100));
    offsets.emplace_back(new OffsetInfo("3", 300));
    offsets.emplace_back(new OffsetInfo("6", 600));
    offsets.emplace_back(new OffsetInfo("8", 800));
    offsets.emplace_back(new OffsetInfo("2", 200));

    std::unordered_map<std::string, OffsetInfoPtr> offsetMap;
    for (const OffsetInfoPtr& offset : offsets)
    {
        offsetMap[offset->mShardId] = offset;
        sMinHeap.Push(offset);
    }

    offsetMap["3"]->mValid = false;
    offsetMap["5"]->mValid = false;

    int64_t oldTimestamp = -1l;
    while (!sMinHeap.Empty()) {
        const OffsetInfoPtr& info = sMinHeap.Top();
        ASSERT_TRUE(oldTimestamp <= info->mTimestamp);
        if (info->mShardId == "3" || info->mShardId == "5")
        {
            ASSERT_FALSE(info->mValid);
        }
        else
        {
            ASSERT_TRUE(info->mValid);
        }
        oldTimestamp = info->mTimestamp;
        sMinHeap.Pop();
    }
}
