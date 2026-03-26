#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "datahub/datahub_typedef.h"
#define private public
#define protected public
#include "shard_select_strategy.h"
#include "datahub/datahub_record.h"

#define MAX_SYSTEMTIME 10000000000000
#define MAX_SHARD_ID 2000
#define SHARD_NUM 500

using namespace aliyun;
using namespace aliyun::datahub;


class ShardSelectStrategyGTest : public ::testing::Test
{
protected:
    static ShardReaderSelectStrategyPtr mShardReaderSelector;
};

ShardReaderSelectStrategyPtr ShardSelectStrategyGTest::mShardReaderSelector = nullptr;

TEST_F(ShardSelectStrategyGTest, AddShardGTest)
{
    mShardReaderSelector.reset(new ShardReaderSelectStrategy);

    std::srand((int)time(0));
    StringVec readyToConsumeShards;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        readyToConsumeShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
    }

    const std::set<std::string>& constEmptyShards = mShardReaderSelector->mEmptyShards;
    const std::unordered_map<std::string, OffsetInfoPtr>& constShardOffsetMap = mShardReaderSelector->mShardOffsetMap;

    std::string minShard = readyToConsumeShards[0];
    int64_t systemtime = (int64_t)rand() % MAX_SYSTEMTIME;
    for (size_t i = 0; i < readyToConsumeShards.size(); i++)
    {
        mShardReaderSelector->AddShard(readyToConsumeShards[i], systemtime);
        ASSERT_TRUE(constEmptyShards.count(readyToConsumeShards[i]) == 0);
        ASSERT_TRUE(constShardOffsetMap.count(readyToConsumeShards[i]) > 0);
        ASSERT_EQ(constShardOffsetMap.at(readyToConsumeShards[i])->mTimestamp, systemtime);
        systemtime ++;
    }

    const std::string& shardId = mShardReaderSelector->GetNextShard();
    ASSERT_EQ(shardId, minShard);
}

TEST_F(ShardSelectStrategyGTest, RemoveShardGTest)
{
    mShardReaderSelector.reset(new ShardReaderSelectStrategy);

    std::srand((int)time(0));
    StringVec readEndShards;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        readEndShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
    }

    // some shard ready to consume
    for (int i = 0; i < SHARD_NUM; i++)
    {
        std::string shard = std::to_string(rand() % MAX_SHARD_ID);
        int64_t systemtime = (int64_t)(rand() % MAX_SYSTEMTIME);
        mShardReaderSelector->mShardOffsetMap[shard].reset(new OffsetInfo(shard, systemtime));
    }

    const std::set<std::string>& constEmptyShards = mShardReaderSelector->mEmptyShards;
    const std::unordered_map<std::string, OffsetInfoPtr>& constShardOffsetMap = mShardReaderSelector->mShardOffsetMap;

    for (size_t i = 0; i < readEndShards.size(); i++)
    {
        mShardReaderSelector->RemoveShard(readEndShards[i]);
        ASSERT_TRUE(constEmptyShards.count(readEndShards[i]) == 0);
        ASSERT_TRUE(constShardOffsetMap.count(readEndShards[i]) == 0);
    }
}

TEST_F(ShardSelectStrategyGTest, GetNextShardWithoutEmptyShardGTest)
{
    mShardReaderSelector.reset(new ShardReaderSelectStrategy);

    std::srand((int)time(0));
    StringVec expectOrderedShards;
    int64_t systemtime = -1;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        std::string shardId = std::to_string(rand() % MAX_SHARD_ID);
        if (mShardReaderSelector->mShardOffsetMap.count(shardId) == 0)
        {
            mShardReaderSelector->AddShard(shardId, ++systemtime);
            expectOrderedShards.push_back(shardId);
        }
    }

    for (size_t i = 0; i < expectOrderedShards.size(); i++)
    {
        const std::string& selectedShardId = mShardReaderSelector->GetNextShard();
        ASSERT_EQ(selectedShardId, expectOrderedShards[i]);
        RecordResultPtr result(new RecordResult());
        result->mSystemTime = ++systemtime;
        mShardReaderSelector->AfterRead(selectedShardId, result);
    }
}

TEST_F(ShardSelectStrategyGTest, GetNextShardWithEmptyShardGTest)
{
    mShardReaderSelector.reset(new ShardReaderSelectStrategy);

    std::srand((int)time(0));
    StringVec expectOrderedShards, emptyShards;
    int64_t systemtime = -1;

    // Select some read empty shard
    for (int i = 0; i < (int)(SHARD_NUM / 5); i++)
    {
        std::string shardId = std::to_string(rand() % MAX_SHARD_ID);
        if (std::find(emptyShards.begin(), emptyShards.end(), shardId) == emptyShards.end())
        {
            mShardReaderSelector->AddShard(shardId);
            emptyShards.push_back(shardId);
        }
    }

    for (int i = 0; i < SHARD_NUM; i++)
    {
        std::string shardId = std::to_string(rand() % MAX_SHARD_ID);
        if (mShardReaderSelector->mShardOffsetMap.count(shardId) == 0)
        {
            mShardReaderSelector->AddShard(shardId, ++systemtime);
            if (std::find(emptyShards.begin(), emptyShards.end(), shardId) == emptyShards.end())
            {
                expectOrderedShards.push_back(shardId);
            }
        }
    }

    for (const std::string& shard : emptyShards)
    {
        mShardReaderSelector->AfterRead(shard, nullptr);
    }

    for (size_t i = 0; i < expectOrderedShards.size(); i++)
    {
        const std::string& selectedShardId = mShardReaderSelector->GetNextShard();
        ASSERT_EQ(selectedShardId, expectOrderedShards[i]);
        mShardReaderSelector->AfterRead(selectedShardId, nullptr);
    }
}

TEST_F(ShardSelectStrategyGTest, AfterReadShardGTest)
{
    mShardReaderSelector.reset(new ShardReaderSelectStrategy);

    std::srand((int)time(0));
    std::map<std::string, RecordResultPtr> readSuccessShardId;
    StringVec readNullShardId;

    for (int i = 0; i < SHARD_NUM; i++)
    {
        RecordResultPtr recordResult(new RecordResult());
        recordResult->mSystemTime = (int64_t)(rand() % MAX_SYSTEMTIME);
        std::string successShard = std::to_string(rand() % MAX_SHARD_ID);
        readSuccessShardId[successShard] = recordResult;

        std::string nullShard = std::to_string(rand() % MAX_SHARD_ID);
        readNullShardId.push_back(nullShard);

        mShardReaderSelector->AddShard(successShard);
        mShardReaderSelector->AddShard(nullShard);
    }

    const std::set<std::string>& constEmptyShards = mShardReaderSelector->mEmptyShards;
    const std::unordered_map<std::string, OffsetInfoPtr>& constShardOffsetMap = mShardReaderSelector->mShardOffsetMap;

    std::map<std::string, RecordResultPtr>::iterator successIt = readSuccessShardId.begin();
    StringVec::iterator nullIt = readNullShardId.begin();
    while (1)
    {
        if (successIt == readSuccessShardId.end() && nullIt == readNullShardId.end())
        {
            break;
        }
        if (successIt != readSuccessShardId.end())
        {
            mShardReaderSelector->AfterRead(successIt->first, successIt->second);
            ASSERT_TRUE(constShardOffsetMap.count(successIt->first) > 0);
            ASSERT_EQ(constShardOffsetMap.at(successIt->first)->mTimestamp, successIt->second->GetSystemTime());
            successIt ++;
        }
        if (nullIt != readNullShardId.end())
        {
            mShardReaderSelector->AfterRead(*nullIt, nullptr);
            ASSERT_TRUE(constEmptyShards.count(*nullIt) > 0);
            nullIt ++;
        }
    }
}
