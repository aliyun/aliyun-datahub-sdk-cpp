#include "meta_data.h"
#include "shard_writer.h"
#include "shard_coordinator.h"
#include "client/producer/producer_config.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#define private public
#include "meta_cache.h"
#include "datahub_client_factory.h"
#include "shard_group_writer.h"
#include "record_pack_queue.h"

#define TEST_NUM 500
#define MAX_EPOCH_NUM 500

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class ShardGroupWriterGTest : public Test
{
protected:
    virtual void SetUp()
    {
        account.id = "testAccessId";
        account.key = "testAccesskey";
        endpoint = "http://successEndpoint";
        projectName = "testProject";
        topicName = "testTopic";
        subId = "";

        datahubClientPtr = std::make_shared<GMockDatahubClient>(Configuration(account, endpoint));
        std::string key;
        key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;

        EXPECT_CALL(*datahubClientPtr.get(), GetTopic(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetGetTopicResult()));
        EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
        std::string metaKey;
        metaKey.append(endpoint).append(":").append(projectName).append(":").append(topicName).append(":").append(subId);
        MetaDataPtr metaDataPtr = std::make_shared<MetaData>(projectName, topicName, subId, CommonConfiguration(account, endpoint));
        metaDataPtr->Init();
        MetaCache::GetInstance().mMetaDataCache[metaKey] = metaDataPtr;
    }

    virtual void TearDown()
    {
        delete shardGroupWriterPtr;
        delete coordinatorPtr;

        MetaCache::GetInstance().mMetaDataCache.clear();
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap.clear();

        datahubClientPtr.reset();
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static std::string subId;
    static ShardCoordinator* coordinatorPtr;
    static ShardGroupWriter* shardGroupWriterPtr;
    static GMockDatahubClientPtr datahubClientPtr;
};

Account ShardGroupWriterGTest::account;
std::string ShardGroupWriterGTest::endpoint;
std::string ShardGroupWriterGTest::projectName;
std::string ShardGroupWriterGTest::topicName;
std::string ShardGroupWriterGTest::subId;
ShardCoordinator* ShardGroupWriterGTest::coordinatorPtr = nullptr;
ShardGroupWriter* ShardGroupWriterGTest::shardGroupWriterPtr = nullptr;
GMockDatahubClientPtr ShardGroupWriterGTest::datahubClientPtr = nullptr;


TEST_F(ShardGroupWriterGTest, CreateShardWriterWhenInitWithSpecialShardsTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"0", "1"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;

    ASSERT_EQ(shardWriterMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMap.count(*it) > 0);
    }
}

TEST_F(ShardGroupWriterGTest, CreateShardWriterWhenInitWithEmptyTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds;
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    ASSERT_EQ(shardGroupWriterPtr->mShardWriterMap.size(), 3u);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;
    for (size_t i = 0; i < 3; i++)
    {
        ASSERT_TRUE(shardWriterMap.count(std::to_string(i)) > 0);
    }
}

TEST_F(ShardGroupWriterGTest, GetNextWriterWithNullShardIdTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"1", "0", "2"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    ASSERT_EQ(shardGroupWriterPtr->mShardWriterMap.size(), shardIds.size());

    for (size_t i = 0; i < TEST_NUM; i++)
    {
        auto writer = shardGroupWriterPtr->GetNextWriter();
        ASSERT_EQ(writer->GetShardId(), shardIds[i % shardIds.size()]);
    }
}

TEST_F(ShardGroupWriterGTest, CreateShardWriterTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"1"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), 1u);
    ASSERT_TRUE(shardWriterMap.count("1") > 0);

    StringVec createShardIds{"0", "2"};
    shardGroupWriterPtr->CreateShardWriter(createShardIds);

    const auto& shardWriterMapAfterCreate = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMapAfterCreate.size(), 3u);
    for (auto it = createShardIds.begin(); it != createShardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMapAfterCreate.count(*it) > 0);
    }
}

TEST_F(ShardGroupWriterGTest, RemoveShardWriterTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"0", "1", "2"};
    StringVec removeShardIds{"0", "2"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMap.count(*it) > 0);
    }

    shardGroupWriterPtr->RemoveShardWriter(removeShardIds);

    const auto& shardWriterMapAfterRemove = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), shardIds.size() - removeShardIds.size());
    for (auto it = removeShardIds.begin(); it != removeShardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMapAfterRemove.count(*it) == 0);
    }
}

TEST_F(ShardGroupWriterGTest, RemoveAllShardWriterTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"0", "1", "2"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    ASSERT_EQ(shardGroupWriterPtr->mShardWriterMap.size(), shardIds.size());

    shardGroupWriterPtr->RemoveAllShardWriter();
    ASSERT_EQ(shardGroupWriterPtr->mShardWriterMap.size(), 0u);
}

TEST_F(ShardGroupWriterGTest, OnShardChangeTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"1", "0"};
    StringVec addShardIds{"2"};
    StringVec delShardIds{"0"};
    StringVec expectShardIds{"1", "2"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMap.count(*it) > 0);
    }

    shardGroupWriterPtr->OnShardChange(addShardIds, delShardIds);

    const auto& shardWriterMapAfterChange = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), expectShardIds.size());
    for (auto it = expectShardIds.begin(); it != expectShardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMapAfterChange.count(*it) > 0);
    }
}

// Remove has a higher priority
TEST_F(ShardGroupWriterGTest, OnShardChangeWithRepeatShardTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"1", "0", "2"};
    StringVec addShardIds{"2", "1"};
    StringVec delShardIds{"2", "0"};
    StringVec expectShardIds{"1"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMap.count(*it) > 0);
    }

    shardGroupWriterPtr->OnShardChange(addShardIds, delShardIds);

    const auto& shardWriterMapAfterChange = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), expectShardIds.size());
    for (auto it = expectShardIds.begin(); it != expectShardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMapAfterChange.count(*it) > 0);
    }
}

TEST_F(ShardGroupWriterGTest, OnRemoveAllShardsTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds{"1", "0", "2"};
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    const auto& shardWriterMap = shardGroupWriterPtr->mShardWriterMap;
    ASSERT_EQ(shardWriterMap.size(), shardIds.size());
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        ASSERT_TRUE(shardWriterMap.count(*it) > 0);
    }

    shardGroupWriterPtr->OnRemoveAllShards();

    ASSERT_EQ(shardGroupWriterPtr->mShardWriterMap.size(), 0u);
}

TEST_F(ShardGroupWriterGTest, WriteTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds;
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    size_t recordNum = 1, epochNum = 2300;
    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    EXPECT_CALL(*datahubClientPtr.get(), PutRecordByShard(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(epochNum);
    for (size_t i = 0 ; i < epochNum; i++)
    {
        shardGroupWriterPtr->Write(records);
    }
}

TEST_F(ShardGroupWriterGTest, WriteAsyncTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetListShardResult()));

    StringVec shardIds;
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    coordinatorPtr = new ShardCoordinator(projectName, topicName, subId, (CommonConfiguration)producerConf);
    shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);

    size_t recordNum = 1, epochNum = 2300;
    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    EXPECT_CALL(*datahubClientPtr.get(), PutRecordByShard(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1));
    for (size_t i = 0 ; i < epochNum; i++)
    {
        shardGroupWriterPtr->WriteAsync(records);
    }
    shardGroupWriterPtr->Flush();
}
