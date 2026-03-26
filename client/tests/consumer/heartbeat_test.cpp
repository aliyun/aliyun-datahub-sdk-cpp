#include "config.h"
#include "consumer_coordinator.h"
#include "client/consumer/consumer_config.h"
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <functional>
#define private public
#include "consumer_heartbeat.h"

using namespace aliyun;
using namespace aliyun::datahub;

class HeartbeatGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        ConsumerConfiguration consumerConf(account, conf_endpoint);
        consumerConf.SetEnableProtobuf(true);
        consumerConf.SetLogFilePath("./DatahubClientTest.log");
        consumerConf.SetFetchLimitNum(1000);
        client = new DatahubClient((Configuration)consumerConf);

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        CreateSubscription(client, conf_project, conf_topic);

        coordinator = new ConsumerCoordinator(conf_project, conf_topic, subId, consumerConf);
        heartbeat = new ConsumerHeartbeat(conf_project, conf_topic, subId, coordinator->GetSessionTimeout(), (Configuration)consumerConf, coordinator);
    }

    static void CreateProject(DatahubClient* client, const std::string& project)
    {
        try
        {
            client->CreateProject(project, "test project");
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void CreateTopic(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        try
        {
            client->CreateTopic(project, topic, conf_shard_count, conf_life_cycle, BLOB, "test topic");
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void CreateSubscription(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        try
        {
            const CreateSubscriptionResult& csr = client->CreateSubscription(project, topic, "test subscription");
            std::cout << "Created subId: " << csr.GetSubId() << ", project:" << project << ", topic: " << topic << std::endl;
            subId = csr.GetSubId();
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void DeleteTopic(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        ListSubscriptionResult lsr = client->ListSubscription(project, topic, 1, 10);
        const std::vector<SubscriptionEntry>& ls = lsr.GetSubscriptions();
        for (auto subIt = ls.begin(); subIt != ls.end(); subIt++)
        {
            try
            {
                client->DeleteSubscription(project, topic, subIt->GetSubId());
            }
            catch (const DatahubException& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
        try
        {
            client->DeleteTopic(project, topic);
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void DeleteProject(DatahubClient* client, const std::string& project)
    {
        ListTopicResult ltr = client->ListTopic(project);
        const StringVec& lt = ltr.GetTopicNames();
        for (auto it = lt.begin(); it != lt.end(); it++)
        {
            DeleteTopic(client, project, *it);
        }
        try
        {
            client->DeleteProject(project);
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    virtual void TearDown()
    {
        delete heartbeat;
        delete coordinator;
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
        delete client;
    }

    static std::string subId;
    static DatahubClient* client;
    static ConsumerHeartbeat* heartbeat;
    static ConsumerCoordinator* coordinator;
};

std::string HeartbeatGTest::subId;
DatahubClient* HeartbeatGTest::client;
ConsumerHeartbeat* HeartbeatGTest::heartbeat;
ConsumerCoordinator* HeartbeatGTest::coordinator;

TEST_F(HeartbeatGTest, HeartbeatTest)
{
    const auto& oldAssignShards = heartbeat->mCurrShards;
    ASSERT_EQ(oldAssignShards.size(), 0lu);

    sleep(20);

    StringVec shards;
    for (int i = 0; i < conf_shard_count; i++)
    {
        shards.push_back(std::to_string(i));
    }

    const auto& newAssignShards = heartbeat->mCurrShards;
    ASSERT_EQ(newAssignShards.size(), (uint64_t)conf_shard_count);
    for (auto it = shards.begin(); it != shards.end(); it++)
    {
        ASSERT_TRUE(std::find(newAssignShards.begin(), newAssignShards.end(), *it) != newAssignShards.end());
    }
}
