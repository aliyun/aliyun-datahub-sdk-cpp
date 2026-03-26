#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>
#include "gtest/gtest.h"
#include "datahub/datahub_client.h"
#include "config.h"
#include <iostream>
#include <vector>

using namespace aliyun;
using namespace aliyun::datahub;


class ConsumerGTest: public testing::Test
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

DatahubClient* ConsumerGTest::client;

TEST_F(ConsumerGTest, ConsumerTest)
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
    const CreateSubscriptionResult& result = client->CreateSubscription(conf_project, topic, subscriptionComment);
    std::cout<<"SubId:" << result.GetSubId()<<std::endl;

    const ListSubscriptionResult& subscriptionResult = client->ListSubscription(conf_project, topic, 1, 20);
    ASSERT_EQ(1l, subscriptionResult.GetTotalCount());

    const std::vector<SubscriptionEntry>& subscriptions = subscriptionResult.GetSubscriptions();
    ASSERT_EQ(1u, subscriptions.size());

    std::string subId = result.GetSubId();

    // in case of exception: group is in recovery
    try
    {
        client->JoinGroup(conf_project, topic, subId, 5000);
    }
    catch (...)
    {
        sleep(2);
    }

    const JoinGroupResult& joinGroupResult = client->JoinGroup(conf_project, topic, subId, 5000);

    std::cout<<"ConsumerId:" << joinGroupResult.GetConsumerId()<<std::endl;

    std::vector<std::string> holdShardList;
    holdShardList.push_back("0");
    holdShardList.push_back("1");

    std::vector<std::string> readEndShardList;
    holdShardList.push_back("2");

    const HeartbeatResult& heartbeatResult = client->Heartbeat(conf_project, topic, subId, joinGroupResult.GetConsumerId(),
            joinGroupResult.GetVersionId(), holdShardList, readEndShardList);
     std::cout<<"GetPlanVersion:" << heartbeatResult.GetPlanVersion()<<std::endl;
    // std::cout<<"GetTotalPlan:" << heartbeatResult.GetTotalPlan()<<std::endl;

    std::vector<std::string> releaseShardList;
    releaseShardList.push_back("0");
    releaseShardList.push_back("1");

    client->SyncGroup(conf_project, topic, subId, joinGroupResult.GetConsumerId(), joinGroupResult.GetVersionId(), releaseShardList, readEndShardList);

    client->LeaveGroup(conf_project, topic, subId, joinGroupResult.GetConsumerId(), joinGroupResult.GetVersionId());

    for (size_t i = 0; i < subscriptions.size(); ++i)
    {
        SubscriptionEntry entry = subscriptions[i];

        client->DeleteSubscription(conf_project, topic, entry.GetSubId());
    }

    client->DeleteTopic(conf_project, topic);
}