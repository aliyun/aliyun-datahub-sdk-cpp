#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>
#include "gtest/gtest.h"
#include "datahub/datahub_client.h"
#include "config.h"

using namespace aliyun;
using namespace aliyun::datahub;

class ShardGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        Configuration conf(account, conf_endpoint);
        client = new DatahubClient(conf);
        try
        {
            client->CreateProject(conf_project, "project for topic gtest");
        }
        catch (std::exception& e)
        {
        }
    }

    virtual void TearDown()
    {
        try
        {
            client->DeleteProject(conf_project);
        }
        catch (std::exception& e)
        {
        }
        delete client;
    }

    static void CreateTopic(const std::string& topic)
    {
        const std::string& fieldName = "test";
        RecordSchema schema;
        schema.AddField(Field(fieldName, BIGINT));

        int lifeCycle = 7;
        const std::string comment = "test";
        RecordType type = TUPLE;
        try
        {
            client->CreateTopic(conf_project, topic, shardCount, lifeCycle, type, schema, comment);
        }
        catch (std::exception& e)
        {
        }
    }

    static uint32_t shardCount;
    static DatahubClient* client;
};

uint32_t ShardGTest::shardCount = 3;
DatahubClient* ShardGTest::client;

TEST_F(ShardGTest, ListShardTest)
{
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    try
    {
        CreateTopic(topic);
    }
    catch (std::exception& e)
    {
    }
    ListShardResult result = client->ListShard(conf_project, topic);

    ASSERT_EQ(shardCount, result.GetShards().size());

    client->DeleteTopic(conf_project, topic);
}

TEST_F(ShardGTest, SplitShardTest)
{
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    try
    {
        CreateTopic(topic);
        client->WaitForShardReady(conf_project, topic);
    }
    catch (std::exception& e)
    {
    }
    sleep(5);
    SplitShardResult ssr = client->SplitShard(conf_project, topic, "0", "00000000000000000000000000AAAAAA");
    ASSERT_EQ(ssr.GetChildShards().size(), 2u);

    ListShardResult lsr = client->ListShard(conf_project, topic);

    ASSERT_EQ(shardCount + 2, lsr.GetShards().size());

    client->DeleteTopic(conf_project, topic);
}

TEST_F(ShardGTest, MergeShardTest)
{
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    try
    {
        CreateTopic(topic);
        client->WaitForShardReady(conf_project, topic);
    }
    catch (std::exception& e)
    {
    }

    std::string beginKey;
    std::string endKey;
    ListShardResult lsr = client->ListShard(conf_project, topic);
    std::vector<ShardEntry> shards = lsr.GetShards();
    for (size_t i = 0; i < shards.size(); ++i)
    {
        if (shards[i].GetShardId() == "0")
        {
            beginKey = shards[i].GetBeginHashKey();
        }
        if (shards[i].GetShardId() == "1")
        {
            endKey = shards[i].GetEndHashKey();
        }
    }
    sleep(5);
    MergeShardResult msr = client->MergeShard(conf_project, topic, "0", "1");
    ASSERT_EQ(msr.GetChildShard().GetShardId(), "3");
    ASSERT_EQ(msr.GetChildShard().GetBeginHashKey(), beginKey);
    ASSERT_EQ(msr.GetChildShard().GetEndHashKey(), endKey);

    client->DeleteTopic(conf_project, topic);
}
