#include "config.h"
#include "timer.h"
#include "shard_reader.h"
#include "message_reader.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"
#include "client/consumer/consumer_config.h"
#include "gtest/gtest.h"
#include <map>
#include <string>
#include <iostream>

#define EPOCH 100
#define RECORD_NUM 100

using namespace aliyun;
using namespace aliyun::datahub;

class ShardReaderGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        ConsumerConfiguration conf(account, conf_endpoint);
        conf.SetEnableProtobuf(true);
        client = new DatahubClient(conf);

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        CreateSubscription(client, conf_project, conf_topic);

        sleep(1);
        SendRecordOnce(client, conf_project, conf_topic, subId);

        messageReaderPtr = std::make_shared<MessageReader>(2, 100);

        ListShardResult lsr = client->ListShard(conf_project, conf_topic);
        const auto& shards = lsr.GetShards();
        for (auto it = shards.begin(); it != shards.end(); it++)
        {
            ShardReader* shardReader = new ShardReader(conf_project, conf_topic, subId, conf, messageReaderPtr, it->GetShardId(), offsetMeta);
            shardReaderMap[it->GetShardId()] = shardReader;
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
                    client->PutRecordByShard(project, topic, it->GetShardId(), records);
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
        for (auto it=shardReaderMap.begin(); it!=shardReaderMap.end(); it++)
        {
            delete it->second;
        }
        messageReaderPtr.reset();
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
        delete client;
    }

    static std::string subId;
    static ConsumeOffsetMeta offsetMeta;
    static DatahubClient* client;
    static MessageReaderPtr messageReaderPtr;
    static std::map<std::string, ShardReader*> shardReaderMap;
};

std::string ShardReaderGTest::subId;
ConsumeOffsetMeta ShardReaderGTest::offsetMeta;
DatahubClient* ShardReaderGTest::client;
MessageReaderPtr ShardReaderGTest::messageReaderPtr;;
std::map<std::string, ShardReader*> ShardReaderGTest::shardReaderMap;

TEST_F(ShardReaderGTest, ShardReaderTest)
{
    for (auto it = shardReaderMap.begin(); it != shardReaderMap.end(); it++)
    {
        int SHARD_RECORD_NUM = 0;
        ShardReader* shardReader = it->second;
        while (true)
        {
            auto record = shardReader->Read(1000);
            if (record == nullptr)
            {
                break;
            }
            SHARD_RECORD_NUM++;
        }
        ASSERT_EQ(SHARD_RECORD_NUM, EPOCH * RECORD_NUM);
    }
}