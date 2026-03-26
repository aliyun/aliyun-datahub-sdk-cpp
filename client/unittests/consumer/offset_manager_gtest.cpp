#include "logger.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#define private public
#include "offset_manager.h"
#include "datahub_client_factory.h"

#define MAX_SHARD_ID 2000
#define SHARD_NUM 500

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class OffsetManagerGTest: public Test
{
protected:
    virtual void SetUp()
    {
        account.id = "testAccessId";
        account.key = "testAccesskey";
        endpoint = "http://successEndpoint";
        projectName = "testProject";
        topicName = "testTopic";
        subId = "1667792277584ABCDE";

        datahubClientPtr = std::make_shared<GMockDatahubClient>(Configuration(account, endpoint));
        std::string key;
        key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;
    }

    virtual void TearDown()
    {
        delete offsetManagerPtr;
        datahubClientPtr.reset();
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static std::string subId;
    static std::string shardId;
    static OffsetManager* offsetManagerPtr;
    static GMockDatahubClientPtr datahubClientPtr;
};

Account OffsetManagerGTest::account;
std::string OffsetManagerGTest::endpoint;
std::string OffsetManagerGTest::projectName;
std::string OffsetManagerGTest::topicName;
std::string OffsetManagerGTest::subId;
std::string OffsetManagerGTest::shardId;
OffsetManager* OffsetManagerGTest::offsetManagerPtr = nullptr;
GMockDatahubClientPtr OffsetManagerGTest::datahubClientPtr = nullptr;


TEST_F(OffsetManagerGTest, UpdateOffsetMetaTest)
{
    Configuration conf(account, endpoint);
    offsetManagerPtr = new OffsetManager(projectName, topicName, subId, conf);

    std::srand((int)time(0));
    for (size_t i = 0; i < SHARD_NUM; i++)
    {
        std::string shardId = std::to_string(rand() % MAX_SHARD_ID);
        int64_t version = rand();
        int64_t sessionId = rand();
        offsetManagerPtr->UpdateOffsetMeta(shardId, version, sessionId);

        ASSERT_EQ(offsetManagerPtr->mOffsetMetaMap.count(shardId), 1u);
        ASSERT_EQ(offsetManagerPtr->mOffsetMetaMap.at(shardId).GetVersion(), version);
        ASSERT_EQ(offsetManagerPtr->mOffsetMetaMap.at(shardId).GetSessionId(), sessionId);
        ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.count(shardId), 1u);
    }
}

TEST_F(OffsetManagerGTest, SendOffsetRequestMetaTest)
{
    Configuration conf(account, endpoint);
    offsetManagerPtr = new OffsetManager(projectName, topicName, subId, conf);

    std::srand((int)time(0));
    int64_t version = rand(), sessionId = rand();
    for (size_t i = 0; i < MAX_SHARD_ID; i++)
    {
        offsetManagerPtr->UpdateOffsetMeta(std::to_string(i), version, sessionId);
    }

    int64_t timestamp = 1764418213123l, sequence = 0l;
    for (size_t i = 0; i < MAX_SHARD_ID; i++)
    {
        std::string shardId = std::to_string(i);
        for (size_t j = 0; j < SHARD_NUM; j++)
        {
            std::string nextCursor = "200000000001000000000000aaaabbbb";
            SubscriptionOffset offset(timestamp, sequence);
            MessageKeyPtr messageKey = std::make_shared<MessageKey>(shardId, nextCursor, offset);
            offsetManagerPtr->SendOffsetRequest(messageKey);

            ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.count(shardId), 1u);
            const auto& requestQueue = offsetManagerPtr->mOffsetRequestMap.at(shardId);
            ASSERT_EQ(requestQueue.size(), j + 1);
            const auto& tmp = requestQueue.front();
            ASSERT_EQ(tmp->GetNextCursor(), nextCursor);
            ASSERT_EQ(tmp->GetOffset().GetSequence(), offset.GetSequence());
            ASSERT_EQ(tmp->GetOffset().GetTimestamp(), offset.GetTimestamp());
        }
    }
}

TEST_F(OffsetManagerGTest, ForceCommitOffsetTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), UpdateSubscriptionOffset(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1));
    Configuration conf(account, endpoint);
    offsetManagerPtr = new OffsetManager(projectName, topicName, subId, conf);

    std::srand((int)time(0));
    int64_t version = rand(), sessionId = rand();
    for (size_t i = 0; i < MAX_SHARD_ID; i++)
    {
        offsetManagerPtr->UpdateOffsetMeta(std::to_string(i), version, sessionId);
    } 

    for (size_t i = 0; i < MAX_SHARD_ID; i++)
    {
        int64_t timestamp = rand(), sequence = rand();
        std::string shardId = std::to_string(i);
        for (size_t j = 0; j < SHARD_NUM; j++)
        {
            std::string nextCursor = "200000000001000000000000aaaabbbb";
            SubscriptionOffset offset(timestamp, sequence);
            MessageKeyPtr messageKey = std::make_shared<MessageKey>(shardId, nextCursor, offset);
            messageKey->Ack();
            offsetManagerPtr->SendOffsetRequest(messageKey);
            timestamp ++;
            sequence ++;
        }
    }

    StringVec forceCommitShards;
    for (size_t i = 0; i < SHARD_NUM / 10; i++)
    {
        forceCommitShards.push_back(std::to_string(i));
    }

    for (auto it = forceCommitShards.begin(); it != forceCommitShards.end(); it++)
    {
        ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.count(*it), 1u);
        ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.at(*it).size(), (uint32_t)SHARD_NUM);
    }

    offsetManagerPtr->ForceCommitOffset(forceCommitShards);

    for (auto it = forceCommitShards.begin(); it != forceCommitShards.end(); it++)
    {
        // LastOffsetMap empty
        ASSERT_EQ(offsetManagerPtr->mLastOffsetMap.count(*it), 0u);
        // RequestMap.quque empty
        ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.count(*it), 1u);
        ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.at(*it).size(), 0u);
        // OffsetMetaMap not empty
        ASSERT_EQ(offsetManagerPtr->mOffsetMetaMap.count(*it), 1u);
    }
}

TEST_F(OffsetManagerGTest, OnShardRealeseTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), UpdateSubscriptionOffset(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1));
    Configuration conf(account, endpoint);
    offsetManagerPtr = new OffsetManager(projectName, topicName, subId, conf);

    std::srand((int)time(0));
    int64_t version = rand(), sessionId = rand();
    for (size_t i = 0; i < MAX_SHARD_ID; i++)
    {
        offsetManagerPtr->UpdateOffsetMeta(std::to_string(i), version, sessionId);
    } 

    for (size_t i = 0; i < MAX_SHARD_ID; i++)
    {
        int64_t timestamp = rand(), sequence = rand();
        std::string shardId = std::to_string(i);
        for (size_t j = 0; j < SHARD_NUM; j++)
        {
            std::string nextCursor = "200000000001000000000000aaaabbbb";
            SubscriptionOffset offset(timestamp, sequence);
            MessageKeyPtr messageKey = std::make_shared<MessageKey>(shardId, nextCursor, offset);
            messageKey->Ack();
            offsetManagerPtr->SendOffsetRequest(messageKey);
            timestamp ++;
            sequence ++;
        }
    }

    StringVec realeseShards;
    for (size_t i = 0; i < SHARD_NUM; i++)
    {
        realeseShards.push_back(std::to_string(i));
    }

    offsetManagerPtr->OnShardRealese(realeseShards);

    for (auto it = realeseShards.begin(); it != realeseShards.end(); it++)
    {
        ASSERT_EQ(offsetManagerPtr->mLastOffsetMap.count(*it), 0u);
        ASSERT_EQ(offsetManagerPtr->mOffsetMetaMap.count(*it), 0u);
        ASSERT_EQ(offsetManagerPtr->mOffsetRequestMap.count(*it), 0u);
    }
}
