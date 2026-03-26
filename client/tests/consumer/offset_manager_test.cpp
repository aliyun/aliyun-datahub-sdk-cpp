#include "config.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#define private public
#include "offset_manager.h"

#define EPOCH 100
#define RECORD_NUM 100

using namespace aliyun;
using namespace aliyun::datahub;

class OffsetManagerGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        Configuration conf(account, conf_endpoint);
        conf.SetEnableProtobuf(true);
        client = new DatahubClient(conf);

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        CreateSubscription(client, conf_project, conf_topic);

        sleep(1);
        SendRecordOnce(client, conf_project, conf_topic, subId);

        offsetmanager = new OffsetManager(conf_project, conf_topic, subId, conf);
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
        delete offsetmanager;
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
        delete client;
    }

    static std::string subId;
    static DatahubClient* client;
    static OffsetManager* offsetmanager;
};

std::string OffsetManagerGTest::subId;
DatahubClient* OffsetManagerGTest::client;
OffsetManager* OffsetManagerGTest::offsetmanager;

TEST_F(OffsetManagerGTest, OffsetTest)
{
    int64_t LAST_SEQUENCE = (int64_t)EPOCH * RECORD_NUM;
    std::vector<MessageKeyPtr> messageKeyPtrVec;

    ListShardResult lsr = client->ListShard(conf_project, conf_topic);
    const auto& shardEntryVec = lsr.GetShards();
    StringVec shards;
    for (auto & shardEntry : shardEntryVec)
    {
        shards.push_back(shardEntry.GetShardId());
    }

    OpenSubscriptionOffsetSessionResult osr = client->InitSubscriptionOffsetSession(conf_project, conf_topic, subId, shards);
    const auto& initOffsets = osr.GetOffsets();

    // Update Version & SessionId
    for (auto it = shards.begin(); it != shards.end(); it++)
    {
        ASSERT_TRUE(initOffsets.count(*it) > 0);
        const SubscriptionOffset& subOffset = initOffsets.at(*it);
        offsetmanager->UpdateOffsetMeta(*it, subOffset.GetVersion(), subOffset.GetSessionId());
    }

    // Create messageKey
    for (int64_t id = 0; id < LAST_SEQUENCE; id++)
    {
        for (auto it = shards.begin(); it != shards.end(); it++)
        {
            messageKeyPtrVec.push_back(std::make_shared<MessageKey>(*it, "", SubscriptionOffset(-1, id)));
        }
    }

    // Send messageKey to OffsetManager and Ack
    for (auto it = messageKeyPtrVec.begin(); it != messageKeyPtrVec.end(); it++)
    {
        offsetmanager->SendOffsetRequest(*it);
        (*it)->Ack();
    }

    sleep(5);

    GetSubscriptionOffsetResult gsr = client->GetSubscriptionOffset(conf_project, conf_topic, subId, shards);
    const auto& getOffsets = gsr.GetOffsets();

    ASSERT_EQ(offsetmanager->mLastOffsetMap.size(), 0lu);
    ASSERT_EQ(offsetmanager->mOffsetMetaMap.size(), shards.size());
    ASSERT_EQ(offsetmanager->mOffsetRequestMap.size(), shards.size());
    for (auto it = shards.begin(); it != shards.end(); it++)
    {
        // 验证所有的 OffsetRequest 已提交
        ASSERT_TRUE(offsetmanager->mOffsetRequestMap.count(*it) > 0);
        ASSERT_EQ(offsetmanager->mOffsetRequestMap.at(*it).size(), 0lu);

        // 验证服务端获取到的点位是否正确
        ASSERT_TRUE(getOffsets.count(*it) > 0);
        const SubscriptionOffset& getSubOffset = getOffsets.at(*it);
        ASSERT_EQ(getSubOffset.GetSequence(), LAST_SEQUENCE - 1);
    }
}
