#include "client/topic_meta.h"
#include "shard_meta.h"
#include "datahub/datahub_record.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#define private public
#include "meta_data.h"
#include "datahub_client_factory.h"
#include "gmock/datahub_client_gmock.h"

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class MetaDataGTest : public Test
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
        datahubClientPtr.reset();
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static std::string subId;
    static GMockDatahubClientPtr datahubClientPtr;
};

Account MetaDataGTest::account;
std::string MetaDataGTest::endpoint;
std::string MetaDataGTest::projectName;
std::string MetaDataGTest::topicName;
std::string MetaDataGTest::subId;
GMockDatahubClientPtr MetaDataGTest::datahubClientPtr = nullptr;

TEST_F(MetaDataGTest, UpdateTopicMetaTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetTopic(::testing::_, ::testing::_)).
            Times(1).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetGetTopicResult()));
    CommonConfiguration conf(account, endpoint);

    MetaDataPtr metaData = std::make_shared<MetaData>(projectName, topicName, subId, conf);

    RecordSchema expectSchema;
    std::string fieldName1 = "f1";
    std::string fieldName2 = "f2";
    std::string fieldComment1 = "f1 comment";
    std::string fieldComment2 = "f2 comment";
    expectSchema.AddField(aliyun::datahub::Field(fieldName1, STRING, true, fieldComment1));
    expectSchema.AddField(aliyun::datahub::Field(fieldName2, BIGINT, true, fieldComment2));
    
    sleep(1);
    metaData->InitTopicMeta();
    const TopicMetaPtr& topicMeta = metaData->mTopicMeta;
    ASSERT_EQ(topicMeta->GetEndpoint(), endpoint);
    ASSERT_EQ(topicMeta->GetProjectName(), projectName);
    ASSERT_EQ(topicMeta->GetTopicName(), topicName);
    ASSERT_EQ(topicMeta->GetRecordType(), "TUPLE");
    ASSERT_EQ(topicMeta->GetRecordSchema(), expectSchema);
}

TEST_F(MetaDataGTest, UpdateShardMetaTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
            Times(3).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult1())).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult2())).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult3()));
    CommonConfiguration conf(account, endpoint);

    MetaDataPtr metaData = std::make_shared<MetaData>(projectName, topicName, subId, conf);

    std::vector<std::map<std::string, ShardState> > expectShardVecs;
    std::map<std::string, ShardState> expectShards1 = {
        {"0", ACTIVE},
        {"1", ACTIVE},
        {"2", ACTIVE},
    };
    std::map<std::string, ShardState> expectShards2 = {
        {"0", CLOSED},
        {"1", ACTIVE},
        {"2", ACTIVE},
        {"3", ACTIVE},
        {"4", ACTIVE},
    };
    std::map<std::string, ShardState> expectShards3 = {
        {"0", CLOSED},
        {"1", CLOSED},
        {"2", CLOSED},
        {"3", ACTIVE},
        {"4", ACTIVE},
        {"5", ACTIVE},
    };
    expectShardVecs.push_back(expectShards1);
    expectShardVecs.push_back(expectShards2);
    expectShardVecs.push_back(expectShards3);

    for (int i = 0; i < 3; i++)
    {
        metaData->UpdateShardMetaMap();
        const std::map<std::string, ShardMetaPtr>& shardMetaPtrMap = metaData->mShardMetaMap;
        const auto& expectShards = expectShardVecs[i];
        ASSERT_EQ(shardMetaPtrMap.size(), expectShards.size());
        for (auto it = expectShards.begin(); it != expectShards.end(); it++)
        {
            ASSERT_EQ(shardMetaPtrMap.count(it->first), 1u);
            const ShardMetaPtr& shardMeta = shardMetaPtrMap.at(it->first);
            ASSERT_EQ(shardMeta->GetShardId(), it->first);
            ASSERT_EQ(shardMeta->GetShardState(), it->second);
        }
        sleep(1);
    }
}
