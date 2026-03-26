#include "config.h"
#include "client/consumer/consumer_config.h"
#include "client/consumer/datahub_consumer.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <iostream>

#define EPOCH 100
#define RECORD_NUM 100

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

        ConsumerConfiguration consumerConf(account, conf_endpoint);
        consumerConf.SetEnableProtobuf(true);
        consumerConf.SetLogFilePath("./DatahubClientTest.log");
        client = new DatahubClient((Configuration)consumerConf);

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        CreateSubscription(client, conf_project, conf_topic);

        sleep(1);
        SendRecordOnce(client, conf_project, conf_topic, subId);

        try
        {
            consumer = new DatahubConsumer(conf_project, conf_topic, subId, consumerConf);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

    void SendRecordOnce(DatahubClient* client, const std::string& project, const std::string& topic, const std::string& subId)
    {
        ListShardResult lsr = client->ListShard(project, topic);
        auto shards = lsr.GetShards();

        std::vector<RecordEntry> records;
        for (int i = 0; i < RECORD_NUM; i++)
        {
            RecordEntry record = RecordEntry(BLOB);
            record.SetData("test record for consumer gtest " + std::to_string(i));
            records.push_back(record);
        }
        sleep(5);
        for (int epoch = 0; epoch < EPOCH; epoch++)
        {
            for (auto it = shards.begin(); it != shards.end(); it++)
            {
                if (it->GetState() == ACTIVE)
                {
                    // client->PutRecordByShard(project, topic, it->GetShardId(), records);
                    client->PutRecord(project, topic, records);
                }
            }
        }
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
        delete consumer;
        try
        {
            client->DeleteSubscription(conf_project, conf_topic, subId);
        }
        catch (const std::exception& e)
        {
        }
        try
        {
            client->DeleteTopic(conf_project, conf_topic);
        }
        catch (const std::exception& e)
        {
        }
        try
        {
            client->DeleteProject(conf_project);
        }
        catch (const std::exception& e)
        {
        }
        delete client;
    }

    static std::string subId;
    static DatahubClient* client;
    static DatahubConsumer* consumer;
};

std::string ConsumerGTest::subId;
DatahubClient* ConsumerGTest::client;
DatahubConsumer* ConsumerGTest::consumer;

TEST_F(ConsumerGTest, ConsumerTest)
{
    int total_Num = 0;
    while (true)
    {
        auto record = consumer->Read("", INT64_MAX);
        if (record == nullptr)
        {
            break;
        }
        total_Num++;
    }
    ASSERT_EQ(total_Num, EPOCH * RECORD_NUM * conf_shard_count);
}
