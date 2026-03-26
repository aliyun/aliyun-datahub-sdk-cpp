#include "client/producer/producer_config.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#define private public
#include "meta_cache.h"
#include "datahub_client_factory.h"
#include "shard_writer.h"
#include "record_pack_queue.h"
#include "message_writer.h"
#include "shard_select_strategy.h"

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class ShardWriterGTest : public Test
{
protected:
    virtual void SetUp()
    {
        account.id = "testAccessId";
        account.key = "testAccesskey";
        endpoint = "http://successEndpoint";
        projectName = "testProject";
        topicName = "testTopic";

        datahubClientPtr = std::make_shared<GMockDatahubClient>(Configuration(account, endpoint));
        std::string key;
        key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;

        messageWriter.reset(new MessageWriter(2, 128));
        shardSelector.reset(new ShardWriterSelectStrategy());
    }

    virtual void TearDown()
    {
        MetaCache::GetInstance().mMetaDataCache.clear();
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap.clear();

        shardWriterPtr.reset();
        datahubClientPtr.reset();
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static ShardWriterPtr shardWriterPtr;
    static GMockDatahubClientPtr datahubClientPtr;
    static ShardWriterSelectStrategyPtr shardSelector;
    static MessageWriterPtr messageWriter;
};

Account ShardWriterGTest::account;
std::string ShardWriterGTest::endpoint;
std::string ShardWriterGTest::projectName;
std::string ShardWriterGTest::topicName;
ShardWriterPtr ShardWriterGTest::shardWriterPtr = nullptr;
GMockDatahubClientPtr ShardWriterGTest::datahubClientPtr = nullptr;
ShardWriterSelectStrategyPtr ShardWriterGTest::shardSelector = nullptr;
MessageWriterPtr ShardWriterGTest::messageWriter = nullptr;


TEST_F(ShardWriterGTest, WriteTest)
{
    size_t recordNum = 1, epochNum = 23000;
    std::string shardId = "0";
    ProducerConfiguration producerConf(account, endpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("");
    shardWriterPtr.reset(new ShardWriter(projectName, topicName, producerConf, shardId, messageWriter, shardSelector));

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
        shardWriterPtr->Write(records);
    }
}
