#include "config.h"
#include "client/topic_meta.h"
#include "timer.h"
#include "shard_meta.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#define private public
#include "meta_data.h"
#include "meta_cache.h"
#include "datahub_client_factory.h"

using namespace aliyun;
using namespace aliyun::datahub;

class MetaDataGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        account.id = conf_accessId;
        account.key = conf_accessKey;
        CommonConfiguration conf(account, conf_endpoint);
        conf.SetEnableProtobuf(true);
        client = new DatahubClient((Configuration)conf);

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        CreateSubscription(client, conf_project, conf_topic);

        readMetaData.reset(new MetaData(conf_project, conf_topic, subId, conf));
        writeMetaData.reset(new MetaData(conf_project, conf_topic, "", conf));
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
        MetaCache::GetInstance().mMetaDataCache.clear();
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap.clear();
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
        delete client;
    }

    static std::string subId;
    static Account account;
    static MetaDataPtr writeMetaData;
    static MetaDataPtr readMetaData;
    static DatahubClient* client;
};

std::string MetaDataGTest::subId;
Account MetaDataGTest::account;
MetaDataPtr MetaDataGTest::writeMetaData;
MetaDataPtr MetaDataGTest::readMetaData;
DatahubClient* MetaDataGTest::client;

TEST_F(MetaDataGTest, MetaDataWhenReadTest)
{
    {
        // InitTopicMeta
        readMetaData->InitTopicMeta();
        readMetaData->UpdateShardMetaMap();

        writeMetaData->InitTopicMeta();
        writeMetaData->UpdateShardMetaMap();

        TopicMetaPtr topicMeta = readMetaData->GetTopicMeta();
        ASSERT_EQ(topicMeta->GetEndpoint(), conf_endpoint);
        ASSERT_EQ(topicMeta->GetProjectName(), conf_project);
        ASSERT_EQ(topicMeta->GetTopicName(), conf_topic);
        ASSERT_EQ(topicMeta->GetRecordType(), GetNameForRecordType(BLOB));
        ASSERT_EQ(topicMeta->GetRecordSchema().GetFieldCount(), 0);

        topicMeta = writeMetaData->GetTopicMeta();
        ASSERT_EQ(topicMeta->GetEndpoint(), conf_endpoint);
        ASSERT_EQ(topicMeta->GetProjectName(), conf_project);
        ASSERT_EQ(topicMeta->GetTopicName(), conf_topic);
        ASSERT_EQ(topicMeta->GetRecordType(), GetNameForRecordType(BLOB));
        ASSERT_EQ(topicMeta->GetRecordSchema().GetFieldCount(), 0);

        ListShardResult initlsr = client->ListShard(conf_project, conf_topic);
        auto initShardEntryVec = initlsr.GetShards();

        auto initShardMetaMap = readMetaData->GetShardMetaMap();
        for (auto & shardEntry : initShardEntryVec)
        {
            ASSERT_TRUE(initShardMetaMap.count(shardEntry.GetShardId()) > 0);
            ASSERT_EQ(initShardMetaMap.at(shardEntry.GetShardId())->GetShardId(), shardEntry.GetShardId());
        }

        initShardMetaMap = writeMetaData->GetShardMetaMap();
        for (auto & shardEntry : initShardEntryVec)
        {
            ASSERT_TRUE(initShardMetaMap.count(shardEntry.GetShardId()) > 0);
            ASSERT_EQ(initShardMetaMap.at(shardEntry.GetShardId())->GetShardId(), shardEntry.GetShardId());
        }
    }

    {
        // Wait for update state
        client->WaitForShardReady(conf_project, conf_topic);
        readMetaData->mUpdateShardTimer->ResetDeadline();
        readMetaData->UpdateShardMetaMap();

        writeMetaData->mUpdateShardTimer->ResetDeadline();
        writeMetaData->UpdateShardMetaMap();

        ListShardResult newlsr = client->ListShard(conf_project, conf_topic);
        auto newShardEntryVec = newlsr.GetShards();

        auto newShardMetaMap = readMetaData->GetShardMetaMap();
        for (auto & shardEntry : newShardEntryVec)
        {
            ASSERT_EQ(shardEntry.GetState(), ACTIVE);
            ASSERT_TRUE(newShardMetaMap.count(shardEntry.GetShardId()) > 0);
            ASSERT_EQ(newShardMetaMap.at(shardEntry.GetShardId())->GetShardId(), shardEntry.GetShardId());
            ASSERT_EQ(newShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), shardEntry.GetState());
        }

        newShardMetaMap = writeMetaData->GetShardMetaMap();
        for (auto & shardEntry : newShardEntryVec)
        {
            ASSERT_EQ(shardEntry.GetState(), ACTIVE);
            ASSERT_TRUE(newShardMetaMap.count(shardEntry.GetShardId()) > 0);
            ASSERT_EQ(newShardMetaMap.at(shardEntry.GetShardId())->GetShardId(), shardEntry.GetShardId());
            ASSERT_EQ(newShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), shardEntry.GetState());
        }
    }

    {
        // Split "0" --> "3" & "4"
        client->WaitForShardReady(conf_project, conf_topic);
        sleep(5);
        std::string splitShard = "0";
        SplitShardResult splitResult = client->SplitShard(conf_project, conf_topic, splitShard, "0000000000000000AAAAAAAAAAAAAAAA");
        ASSERT_EQ(splitResult.GetChildShards().size(), 2u);
        ASSERT_EQ(splitResult.GetChildShards()[0].GetShardId(), "3");
        ASSERT_EQ(splitResult.GetChildShards()[1].GetShardId(), "4");

        readMetaData->mUpdateShardTimer->ResetDeadline();
        readMetaData->UpdateShardMetaMap();

        writeMetaData->mUpdateShardTimer->ResetDeadline();
        writeMetaData->UpdateShardMetaMap();

        ListShardResult splitlsr = client->ListShard(conf_project, conf_topic);
        auto splitShardEntryVec = splitlsr.GetShards();

        auto splitShardMetaMap = readMetaData->GetShardMetaMap();
        for (auto & shardEntry : splitShardEntryVec)
        {
            ASSERT_TRUE(splitShardMetaMap.count(shardEntry.GetShardId()) > 0);
            if (shardEntry.GetShardId() == "0")
            {
                ASSERT_EQ(splitShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), CLOSED);
            }
            else if (shardEntry.GetShardId() == "3" || shardEntry.GetShardId() == "4")
            {
                ASSERT_EQ(splitShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), OPENING);
            }
            else
            {
                ASSERT_EQ(splitShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), ACTIVE);
            }
        }

        splitShardMetaMap = writeMetaData->GetShardMetaMap();
        for (auto & shardEntry : splitShardEntryVec)
        {
            if (shardEntry.GetShardId() == "0")
            {
                ASSERT_FALSE(splitShardMetaMap.count(shardEntry.GetShardId()) > 0);
                ASSERT_EQ(shardEntry.GetState(), CLOSED);
            }
            else if (shardEntry.GetShardId() == "3" || shardEntry.GetShardId() == "4")
            {
                ASSERT_FALSE(splitShardMetaMap.count(shardEntry.GetShardId()) > 0);
                ASSERT_EQ(shardEntry.GetState(), OPENING);
            }
            else
            {
                ASSERT_TRUE(splitShardMetaMap.count(shardEntry.GetShardId()) > 0);
                ASSERT_EQ(splitShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), ACTIVE);
            }
        }
    }

    {
        // Merge "1" & "2" --> "5"
        sleep(5);
        client->WaitForShardReady(conf_project, conf_topic);
        std::string shardId = "1", adjacentShardId = "2";
        client->MergeShard(conf_project, conf_topic, shardId, adjacentShardId);

        readMetaData->mUpdateShardTimer->ResetDeadline();
        readMetaData->UpdateShardMetaMap();

        writeMetaData->mUpdateShardTimer->ResetDeadline();
        writeMetaData->UpdateShardMetaMap();

        ListShardResult mergelsr = client->ListShard(conf_project, conf_topic);
        auto mergeShardEntryVec = mergelsr.GetShards();

        auto mergeShardMetaMap = readMetaData->GetShardMetaMap();
        for (auto & shardEntry : mergeShardEntryVec)
        {
            ASSERT_TRUE(mergeShardMetaMap.count(shardEntry.GetShardId()) > 0);
            if (shardEntry.GetShardId() == "0" || shardEntry.GetShardId() == "1" || shardEntry.GetShardId() == "2")
            {
                ASSERT_EQ(mergeShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), CLOSED);
            }
            else if (shardEntry.GetShardId() == "5")
            {
                ASSERT_EQ(mergeShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), OPENING);
            }
            else
            {
                ASSERT_EQ(mergeShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), ACTIVE);
            }
        }

        mergeShardMetaMap = writeMetaData->GetShardMetaMap();
        for (auto & shardEntry : mergeShardEntryVec)
        {
            if (shardEntry.GetShardId() == "0" || shardEntry.GetShardId() == "1" || shardEntry.GetShardId() == "2")
            {
                ASSERT_FALSE(mergeShardMetaMap.count(shardEntry.GetShardId()) > 0);
                ASSERT_EQ(shardEntry.GetState(), CLOSED);
            }
            else if (shardEntry.GetShardId() == "5")
            {
                ASSERT_FALSE(mergeShardMetaMap.count(shardEntry.GetShardId()) > 0);
                ASSERT_EQ(shardEntry.GetState(), OPENING);
            }
            else
            {
                ASSERT_TRUE(mergeShardMetaMap.count(shardEntry.GetShardId()) > 0);
                ASSERT_EQ(mergeShardMetaMap.at(shardEntry.GetShardId())->GetShardState(), ACTIVE);
            }
        }
    }
}
