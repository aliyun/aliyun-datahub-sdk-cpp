#include "logger.h"
#include "shard_meta.h"
#include "offset_meta.h"
#include "client/consumer/consumer_config.h"
#include "consumer_coordinator.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#include "gmock/consumer_coordinator_gmock.h"
#define private public
#include "meta_data.h"
#include "shard_group_reader.h"
#include "meta_cache.h"
#include "datahub_client_factory.h"
#include "gmock/meta_data_gmock.h"

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class ShardGroupReaderGTest: public Test
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

        CommonConfiguration conf(account, endpoint);
        metaDataPtr = std::make_shared<GMockMetaData>(projectName, topicName, subId, conf);
        std::string metaKey;
        metaKey.append(endpoint).append(":").append(projectName).append(":").append(topicName).append(":").append(subId);
        metaDataPtr->mShardMetaMap["0"] = std::make_shared<ShardMeta>("0", ACTIVE);
        metaDataPtr->mShardMetaMap["1"] = std::make_shared<ShardMeta>("1", ACTIVE);
        metaDataPtr->mShardMetaMap["2"] = std::make_shared<ShardMeta>("2", ACTIVE);
        MetaCache::GetInstance().mMetaDataCache[metaKey] = metaDataPtr;
    }

    virtual void TearDown()
    {
        shardGroupReaderPtr.reset();
        delete coordinatorPtr;

        metaDataPtr.reset();
        datahubClientPtr.reset();
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static std::string subId;
    static OffsetCoordinator* coordinatorPtr;
    static ShardGroupReaderPtr shardGroupReaderPtr;
    static GMockDatahubClientPtr datahubClientPtr;
    static MetaDataPtr metaDataPtr;
};

Account ShardGroupReaderGTest::account;
std::string ShardGroupReaderGTest::endpoint;
std::string ShardGroupReaderGTest::projectName;
std::string ShardGroupReaderGTest::topicName;
std::string ShardGroupReaderGTest::subId;
OffsetCoordinator* ShardGroupReaderGTest::coordinatorPtr = nullptr;
ShardGroupReaderPtr ShardGroupReaderGTest::shardGroupReaderPtr = nullptr;
GMockDatahubClientPtr ShardGroupReaderGTest::datahubClientPtr = nullptr;
MetaDataPtr ShardGroupReaderGTest::metaDataPtr = nullptr;

TEST_F(ShardGroupReaderGTest, GenShardOffsetTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockOffsetCoordinator(projectName, topicName, subId, consumerConf);

    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, StringVec(), -1l);


    StringVec shardIds{"0", "1", "2"};
    int64_t timestamp = -1l;
    const std::map<std::string, ConsumeOffsetMeta>& offsetMap = shardGroupReaderPtr->GenShardOffset(shardIds, timestamp);

    ASSERT_EQ(offsetMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_EQ(offsetMap.count(*it), 1u);
        ASSERT_EQ(offsetMap.at(*it).GetSequence(), 2l);
        ASSERT_EQ(offsetMap.at(*it).GetTimestamp(), 1l);
        ASSERT_EQ(offsetMap.at(*it).GetVersion(), 1l);
        ASSERT_EQ(offsetMap.at(*it).GetSessionId(), 1l);
        ASSERT_EQ(offsetMap.at(*it).GetNextCursor(), "");
    }
}

TEST_F(ShardGroupReaderGTest, GenShardOffsetWithTimestampTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockOffsetCoordinator(projectName, topicName, subId, consumerConf);

    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, StringVec(), -1l);

    StringVec shardIds{"0", "1", "2"};
    int64_t timestamp = 1667792170345l;
    const std::map<std::string, ConsumeOffsetMeta>& offsetMap = shardGroupReaderPtr->GenShardOffset(shardIds, timestamp);

    ASSERT_EQ(offsetMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_EQ(offsetMap.count(*it), 1u);
        ASSERT_EQ(offsetMap.at(*it).GetSequence(), -1l);
        ASSERT_EQ(offsetMap.at(*it).GetTimestamp(), timestamp);
        ASSERT_EQ(offsetMap.at(*it).GetVersion(), 1l);
        ASSERT_EQ(offsetMap.at(*it).GetSessionId(), 1l);
        ASSERT_EQ(offsetMap.at(*it).GetNextCursor(), "");
    }
}

TEST_F(ShardGroupReaderGTest, OnShardChangeTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockOffsetCoordinator(projectName, topicName, subId, consumerConf);

    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, StringVec(), -1l);

    StringVec addShardIds{"0", "2"};
    StringVec delShardIds{"1"};

    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), 0u);
    shardGroupReaderPtr->OnShardChange(addShardIds, delShardIds);
    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), addShardIds.size());
    for (auto it = addShardIds.begin(); it != addShardIds.begin(); it++)
    {
        ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.count(*it), 1u);
    }
    for (auto it = delShardIds.begin(); it != delShardIds.begin(); it++)
    {
        ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.count(*it), 0u);
    }
}

// Remove has a higher priority
TEST_F(ShardGroupReaderGTest, OnShardChangeWithRepeatShardTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockOffsetCoordinator(projectName, topicName, subId, consumerConf);

    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, StringVec(), -1l);

    StringVec addShardIds{"0", "2"};
    StringVec delShardIds{"1", "2"};
    StringVec expectShardIds{"0"};

    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), 0u);
    shardGroupReaderPtr->OnShardChange(addShardIds, delShardIds);
    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), expectShardIds.size());
    for (auto it = expectShardIds.begin(); it != expectShardIds.begin(); it++)
    {
        ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.count(*it), 1u);
    }
    for (auto it = delShardIds.begin(); it != delShardIds.begin(); it++)
    {
        ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.count(*it), 0u);
    }

}

TEST_F(ShardGroupReaderGTest, OnRemoveAllShardsTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockOffsetCoordinator(projectName, topicName, subId, consumerConf);

    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, StringVec(), -1l);

    StringVec createShardIds{"0", "2"};
    int64_t timestamp = -1l;

    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), 0u);
    shardGroupReaderPtr->CreateShardReader(createShardIds, timestamp);
    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), createShardIds.size());
    for (auto it = createShardIds.begin(); it != createShardIds.begin(); it++)
    {
        ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.count(*it), 1u);
    }

    shardGroupReaderPtr->OnRemoveAllShards();
    ASSERT_EQ(shardGroupReaderPtr->mShardReaderMap.size(), 0u);
}

TEST_F(ShardGroupReaderGTest, ReadTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));
    EXPECT_CALL(*datahubClientPtr.get(), UpdateSubscriptionOffset(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockOffsetCoordinator(projectName, topicName, subId, consumerConf);
    StringVec shardIds{"0"};
    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, shardIds, -1l);

    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetGetRecordResult()));

    int64_t readNum = 0, targetNum = 12345;
    while (true)
    {
        auto recordPtr = shardGroupReaderPtr->Read("", 10000);
        ASSERT_EQ(recordPtr->GetBigint(0), 1465999335123456l);
        ASSERT_EQ(recordPtr->GetInteger(1), 30);
        if (++readNum == targetNum)
        {
            break;
        }
    }
    ASSERT_EQ(readNum, targetNum);
}

TEST_F(ShardGroupReaderGTest, RebalanceReadTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
    EXPECT_CALL(*datahubClientPtr.get(), JoinGroup(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetJoinGroupResult()));
    EXPECT_CALL(*datahubClientPtr.get(), Heartbeat(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1)).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetHeartbeatResultWithNull())).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetHeartbeatResult()));
    EXPECT_CALL(*datahubClientPtr.get(), InitSubscriptionOffsetSession(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetOpenSubscriptionOffsetSessionResult()));
    EXPECT_CALL(*datahubClientPtr.get(), UpdateSubscriptionOffset(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1));

    ConsumerConfiguration consumerConf(account, endpoint);
    consumerConf.SetLogFilePath("");
    coordinatorPtr = new GMockConsumerCoordinator(projectName, topicName, subId, consumerConf);
    shardGroupReaderPtr = std::make_shared<ShardGroupReader>(coordinatorPtr, consumerConf, StringVec(), -1l);
    coordinatorPtr->RegisterOnShardChange(std::bind(&ShardGroupReader::OnShardChange, shardGroupReaderPtr, std::placeholders::_1, std::placeholders::_2));
    coordinatorPtr->RegisterOnRemoveAllShards(std::bind(&ShardGroupReader::OnRemoveAllShards, shardGroupReaderPtr));

    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetGetRecordResult()));

    EXPECT_CALL(*datahubClientPtr.get(), LeaveGroup(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_));

    int64_t readNum = 0, targetNum = 12345;
    while (true)
    {
        auto recordPtr = shardGroupReaderPtr->Read("", 60000);
        if (recordPtr == nullptr)
        {
            break;
        }
        ASSERT_EQ(recordPtr->GetBigint(0), 1465999335123456l);
        ASSERT_EQ(recordPtr->GetInteger(1), 30);
        if (++readNum == targetNum)
        {
            break;
        }
    }
    ASSERT_EQ(readNum, targetNum);
}

