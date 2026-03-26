#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "util.h"
#include "logger.h"
#define private public
#include "sync_group_meta.h"
#include <functional>

#define MAX_SHARD_ID 2000
#define SHARD_NUM 50

using namespace aliyun;
using namespace aliyun::datahub;


class SyncGroupMetaGTest : public ::testing::Test
{
protected:
    static SyncGroupMetaPtr mSyncGroupMetaPtr;
};

SyncGroupMetaPtr SyncGroupMetaGTest::mSyncGroupMetaPtr = nullptr; 

TEST_F(SyncGroupMetaGTest, OnShardReleaseGTest)
{
    mSyncGroupMetaPtr = std::make_shared<SyncGroupMeta>();

    std::srand((int)time(0));
    StringVec readyToReleaseShards;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        readyToReleaseShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
    }

    const StringVec& constReleaseShards = mSyncGroupMetaPtr->mReleaseShards;
    const StringVec& constReadEndShards = mSyncGroupMetaPtr->mReadEndShards;
    const StringVec& constActiveShards = mSyncGroupMetaPtr->mActiveShards;

    mSyncGroupMetaPtr->OnShardRelease(readyToReleaseShards);
    for (size_t i = 0; i < readyToReleaseShards.size(); i++)
    {
        ASSERT_TRUE(std::find(constReleaseShards.begin(), constReleaseShards.end(), readyToReleaseShards[i]) != constReleaseShards.end());
        ASSERT_TRUE(std::find(constReadEndShards.begin(), constReadEndShards.end(), readyToReleaseShards[i]) == constReadEndShards.end());
        ASSERT_TRUE(std::find(constActiveShards.begin(), constActiveShards.end(), readyToReleaseShards[i]) == constActiveShards.end());
    }
}

TEST_F(SyncGroupMetaGTest, OnShardReadEndGTest)
{
    mSyncGroupMetaPtr = std::make_shared<SyncGroupMeta>();

    std::srand((int)time(0));
    StringVec readyToReadEndShards;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        readyToReadEndShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
    }

    const StringVec& constReadEndShards = mSyncGroupMetaPtr->mReadEndShards;

    mSyncGroupMetaPtr->OnShardReadEnd(readyToReadEndShards);
    for (size_t i = 0; i < readyToReadEndShards.size(); i++)
    {
        ASSERT_TRUE(std::find(constReadEndShards.begin(), constReadEndShards.end(), readyToReadEndShards[i]) != constReadEndShards.end());
    }
}

TEST_F(SyncGroupMetaGTest, OnShardReleaseAndReadEndGTest)
{
    mSyncGroupMetaPtr = std::make_shared<SyncGroupMeta>();

    std::srand((int)time(0));
    std::vector<StringVec> readyToReleaseShards, readyToReadEndShards;
    for (int i = 0; i < 10; i++)
    {
        StringVec tmpRelease, tmpReadend;
        for (int j = 0; j < SHARD_NUM; j++)
        {
            tmpRelease.push_back(std::to_string(rand() % MAX_SHARD_ID));
            tmpReadend.push_back(std::to_string(rand() % MAX_SHARD_ID));
        }
        readyToReleaseShards.push_back(tmpRelease);
        readyToReadEndShards.push_back(tmpReadend);
    }

    const StringVec& constReleaseShards = mSyncGroupMetaPtr->mReleaseShards;
    const StringVec& constReadEndShards = mSyncGroupMetaPtr->mReadEndShards;
    const StringVec& constActiveShards = mSyncGroupMetaPtr->mActiveShards;

    std::vector<StringVec>::iterator releaseIt = readyToReleaseShards.begin();
    std::vector<StringVec>::iterator readendIt = readyToReadEndShards.begin();

    while (1)
    {
        if (releaseIt == readyToReleaseShards.end() && readendIt == readyToReadEndShards.end())
        {
            break;
        }
        if (releaseIt != readyToReleaseShards.end())
        {
            mSyncGroupMetaPtr->OnShardRelease(*releaseIt);
            for (auto it = releaseIt->begin(); it != releaseIt->end(); it++)
            {
                ASSERT_TRUE(std::find(constReleaseShards.begin(), constReleaseShards.end(), *it) != constReleaseShards.end());
                ASSERT_TRUE(std::find(constReadEndShards.begin(), constReadEndShards.end(), *it) == constReadEndShards.end());
                ASSERT_TRUE(std::find(constActiveShards.begin(), constActiveShards.end(), *it) == constActiveShards.end());
            }
            releaseIt ++;
        }
        if (readendIt != readyToReadEndShards.end())
        {
            mSyncGroupMetaPtr->OnShardReadEnd(*readendIt);
            for (auto it = readendIt->begin(); it != readendIt->end(); it++)
            {
                ASSERT_TRUE(std::find(constReadEndShards.begin(), constReadEndShards.end(), *it) != constReadEndShards.end());
            }
            readendIt ++;
        }
    }
}

TEST_F(SyncGroupMetaGTest, ClearShardReleaseGTest)
{
    mSyncGroupMetaPtr = std::make_shared<SyncGroupMeta>();

    std::srand((int)time(0));
    StringVec readyToReleaseShards, readyToReadEndShards;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        readyToReleaseShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
        readyToReadEndShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
    }

    const StringVec& constReleaseShards = mSyncGroupMetaPtr->mReleaseShards;

    mSyncGroupMetaPtr->OnShardRelease(readyToReleaseShards);
    mSyncGroupMetaPtr->OnShardReadEnd(readyToReadEndShards);

    ASSERT_NE(constReleaseShards.size(), 0u);
    mSyncGroupMetaPtr->ClearShardRelease();
    ASSERT_EQ(constReleaseShards.size(), 0u);
}

TEST_F(SyncGroupMetaGTest, OnHeartBeatDoneGTest)
{
    mSyncGroupMetaPtr = std::make_shared<SyncGroupMeta>();

    std::srand((int)time(0));
    StringVec hasReadEndShards, heartbeatDoneShards;
    for (int i = 0; i < SHARD_NUM; i++)
    {
        std::string tmp = std::to_string(rand() % MAX_SHARD_ID);
        mSyncGroupMetaPtr->mReadEndShards.push_back(tmp);
        hasReadEndShards.push_back(tmp);
    }
    for (int i = 0; i < SHARD_NUM; i++)
    {
        heartbeatDoneShards.push_back(std::to_string(rand() % MAX_SHARD_ID));
    }

    const StringVec& constReadEndShards = mSyncGroupMetaPtr->mReadEndShards;
    const StringVec& constActiveShards = mSyncGroupMetaPtr->mActiveShards;


    mSyncGroupMetaPtr->OnHeartBeatDone(heartbeatDoneShards);

    StringVec expectActiveShards;
    for (auto it = heartbeatDoneShards.begin(); it != heartbeatDoneShards.end(); it++)
    {
        if (std::find(constReadEndShards.begin(), constReadEndShards.end(), *it) == constReadEndShards.end())
        {
            expectActiveShards.push_back(*it);
        }
    }

    for (auto it = expectActiveShards.begin(); it != expectActiveShards.end(); it++)
    {
        ASSERT_TRUE(std::find(constActiveShards.begin(), constActiveShards.end(), *it) != constActiveShards.end());
    }
    ASSERT_EQ(constActiveShards, expectActiveShards);
}
