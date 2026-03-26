#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>
#include "gtest/gtest.h"
#include "datahub/datahub_client.h"
#include "config.h"
#include <iostream>

using namespace aliyun;
using namespace aliyun::datahub;

class TopicGTest: public testing::Test
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

    static DatahubClient* client;
};

DatahubClient* TopicGTest::client;

TEST_F(TopicGTest, CreateTopicTest)
{
    const std::string& fieldName = "test";
    RecordSchema schema;
    schema.AddField(Field(fieldName + "1", INTEGER));
    schema.AddField(Field(fieldName + "2", BIGINT));
    schema.AddField(Field(fieldName + "3", FLOAT));
    schema.AddField(Field(fieldName + "4", DOUBLE));
    schema.AddField(Field(fieldName + "5", DECIMAL));
    schema.AddField(Field(fieldName + "6", BOOLEAN));
    schema.AddField(Field(fieldName + "7", STRING));
    schema.AddField(Field(fieldName + "8", TIMESTAMP));
    schema.AddField(Field(fieldName + "9", TINYINT));
    schema.AddField(Field(fieldName + "10", SMALLINT));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->DeleteTopic(conf_project, topic);
}

TEST_F(TopicGTest, UpdateTopicTest)
{
    const std::string& fieldName = "test";
    RecordSchema schema;
    schema.AddField(Field(fieldName + "1", INTEGER));
    schema.AddField(Field(fieldName + "2", BIGINT));
    schema.AddField(Field(fieldName + "3", FLOAT));
    schema.AddField(Field(fieldName + "4", DOUBLE));
    schema.AddField(Field(fieldName + "5", DECIMAL));
    schema.AddField(Field(fieldName + "6", BOOLEAN));
    schema.AddField(Field(fieldName + "7", STRING));
    schema.AddField(Field(fieldName + "8", TIMESTAMP));
    schema.AddField(Field(fieldName + "9", TINYINT));
    schema.AddField(Field(fieldName + "10", SMALLINT));
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);

    sleep(5);

    const std::string updateComment = "test1";
    int updateLifecycle = 7;
    client->UpdateTopic(conf_project, topic, updateLifecycle, updateComment);

    GetTopicResult result = client->GetTopic(conf_project, topic);
    ASSERT_EQ(updateLifecycle, result.GetLifecycle());
    ASSERT_EQ(updateComment, result.GetComment());

    client->DeleteTopic(conf_project, topic);
}

TEST_F(TopicGTest, GetAndListTopicTest)
{
    const std::string& fieldName = "test";
    RecordSchema schema;
    schema.AddField(Field(fieldName + "1", INTEGER));
    schema.AddField(Field(fieldName + "2", BIGINT));
    schema.AddField(Field(fieldName + "3", FLOAT));
    schema.AddField(Field(fieldName + "4", DOUBLE));
    schema.AddField(Field(fieldName + "5", DECIMAL));
    schema.AddField(Field(fieldName + "6", BOOLEAN));
    schema.AddField(Field(fieldName + "7", STRING));
    schema.AddField(Field(fieldName + "8", TIMESTAMP));
    schema.AddField(Field(fieldName + "9", TINYINT));
    schema.AddField(Field(fieldName + "10", SMALLINT));
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);

    GetTopicResult result = client->GetTopic(conf_project, topic);

    ASSERT_EQ(shardCount, result.GetShardCount());
    ASSERT_EQ(lifecycle, result.GetLifecycle());
    ASSERT_EQ(GetNameForRecordType(type), result.GetRecordType());
    ASSERT_EQ(result.GetRecordSchema().ToJsonString(), schema.ToJsonString());
    ASSERT_EQ(10, result.GetRecordSchema().GetFieldCount());
    ASSERT_EQ(0, result.GetRecordSchema().GetFieldIndex("test1"));

    ListTopicResult listTopicResult = client->ListTopic(conf_project);
    ASSERT_EQ(listTopicResult.GetTopicNames().size() >= 1, true);

    client->DeleteTopic(conf_project, topic);
}

TEST_F(TopicGTest, CreateBlobTopicTest)
{
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = BLOB;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, comment);
    client->DeleteTopic(conf_project, topic);
}

TEST_F(TopicGTest, GetBlobTopicTest)
{
    std::stringstream ss;
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = BLOB;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, comment);

    GetTopicResult result = client->GetTopic(conf_project, topic);

    ASSERT_EQ(shardCount, result.GetShardCount());
    ASSERT_EQ(lifecycle, result.GetLifecycle());
    ASSERT_EQ(GetNameForRecordType(type), result.GetRecordType());
    ASSERT_EQ(0, result.GetRecordSchema().GetFieldCount());

    client->DeleteTopic(conf_project, topic);
}

TEST_F(TopicGTest, AppendFieldTest)
{
    const std::string& fieldName = "test";
    RecordSchema schema;
    schema.AddField(Field(fieldName + "1", INTEGER, false));
    schema.AddField(Field(fieldName + "2", BIGINT));
    schema.AddField(Field(fieldName + "3", FLOAT));
    schema.AddField(Field(fieldName + "4", DOUBLE));
    schema.AddField(Field(fieldName + "5", DECIMAL));
    schema.AddField(Field(fieldName + "6", BOOLEAN));
    schema.AddField(Field(fieldName + "7", STRING));
    schema.AddField(Field(fieldName + "8", TIMESTAMP));
    schema.AddField(Field(fieldName + "9", TINYINT));
    schema.AddField(Field(fieldName + "10", SMALLINT));
    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);

    const std::string& appendFieldName = "test_append_field";
    const std::string& appendFieldType = GetNameForFieldType(STRING);

    client->AppendField(conf_project, topic, appendFieldName, appendFieldType);

    GetTopicResult result = client->GetTopic(conf_project, topic);

    ASSERT_EQ(11, result.GetRecordSchema().GetFieldCount());
    ASSERT_EQ(10, result.GetRecordSchema().GetFieldIndex(appendFieldName));

    Field field = result.GetRecordSchema().GetField(10);
    ASSERT_EQ(STRING, field.GetFieldType());
    ASSERT_TRUE(field.IsAllowNull());
    ASSERT_FALSE(result.GetRecordSchema().GetField(0).IsAllowNull());

    client->DeleteTopic(conf_project, topic);
}

TEST_F(TopicGTest, SubscriptionTest)
{
    const std::string& fieldName = "test";
    RecordSchema schema;
    schema.AddField(Field(fieldName, BIGINT));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;
    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    const std::string subscriptionComment = "test_subscription";
    CreateSubscriptionResult result = client->CreateSubscription(conf_project, topic, subscriptionComment);

    ListSubscriptionResult subscriptionResult = client->ListSubscription(conf_project, topic, 1, 20, result.GetSubId());
    ASSERT_EQ(1l, subscriptionResult.GetTotalCount());

    std::vector<SubscriptionEntry> subscriptions = subscriptionResult.GetSubscriptions();
    ASSERT_EQ(1u, subscriptions.size());

    for (size_t i = 0; i < subscriptions.size(); ++i)
    {
        SubscriptionEntry entry = subscriptions[i];

        const std::string updateSubscriptionComment = "test_subscription_1";
        client->UpdateSubscription(conf_project, topic, entry.GetSubId(), updateSubscriptionComment);

        GetSubscriptionResult getSubscriptionResult = client->GetSubscription(conf_project, topic, entry.GetSubId());
        ASSERT_EQ(updateSubscriptionComment, getSubscriptionResult.GetComment());
        ASSERT_EQ(SubscriptionState::ONLINE, getSubscriptionResult.GetState());

        client->UpdateSubscriptionState(conf_project, topic, entry.GetSubId(), SubscriptionState::OFFLINE);
        GetSubscriptionResult getSubscriptionResult1 = client->GetSubscription(conf_project, topic, entry.GetSubId());
        ASSERT_EQ(SubscriptionState::OFFLINE, getSubscriptionResult1.GetState());

        client->DeleteSubscription(conf_project, topic, entry.GetSubId());
    }

    ListSubscriptionResult subscriptionResult1 = client->ListSubscription(conf_project, topic, 1, 20);
    ASSERT_EQ(0l, subscriptionResult1.GetTotalCount());

    client->DeleteTopic(conf_project, topic);
}