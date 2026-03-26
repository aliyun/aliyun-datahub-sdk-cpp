#include "config.h"
#include "timer.h"
#include "record_pack_queue.h"
#include "client/producer/producer_config.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"
#include "message_writer.h"
#include "shard_select_strategy.h"
#include "gtest/gtest.h"
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#define private public
#include "shard_writer.h"

#define EPOCH 100
#define RECORD_NUM 100

using namespace aliyun;
using namespace aliyun::datahub;

class ShardWriterGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        ProducerConfiguration producerConf(account, conf_endpoint);
        producerConf.SetEnableProtobuf(true);
        producerConf.SetLogFilePath("");
        client.reset(new DatahubClient((Configuration)producerConf));

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);

        messageWriter.reset(new MessageWriter(2, 128));
        shardSelector.reset(new ShardWriterSelectStrategy());

        client->WaitForShardReady(conf_project, conf_topic);
        ListShardResult lsr = client->ListShard(conf_project, conf_topic);
        const auto& shards = lsr.GetShards();
        for (auto it = shards.begin(); it != shards.end(); it++)
        {
            if (it->GetState() == ACTIVE)
            {
                shardWriterMap[it->GetShardId()] = std::make_shared<ShardWriter>(conf_project, conf_topic, producerConf, it->GetShardId(), messageWriter, shardSelector);
            }
        }
    }

    std::map<std::string, int> ReadRecordEachShard(const DatahubClientPtr& client, const std::string& project, const std::string& topic)
    {
        ListShardResult lsr = client->ListShard(project, topic);
        auto shards = lsr.GetShards();
        int limit = 100;
        std::map<std::string, int> totalNumMap;
        for (auto it = shards.begin(); it != shards.end(); it++)
        {
            int totalNum = 0;
            std::string cursor = client->GetCursor(project, topic, it->GetShardId(), CURSOR_TYPE_OLDEST).GetCursor();
            while (true)
            {
                auto getRecordResult = client->GetRecord(project, topic, it->GetShardId(), cursor, limit);
                if (getRecordResult.GetRecordCount() == 0)
                {
                    break;
                }
                totalNum += getRecordResult.GetRecordCount();
                cursor = getRecordResult.GetNextCursor();
            }
            totalNumMap[it->GetShardId()] = totalNum;
        }
        return totalNumMap;
    }

    static void CreateProject(const DatahubClientPtr& client, const std::string& project)
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

    static void CreateTopic(const DatahubClientPtr& client, const std::string& project, const std::string& topic)
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

    static void DeleteTopic(const DatahubClientPtr& client, const std::string& project, const std::string& topic)
    {
        try
        {
            client->DeleteTopic(project, topic);
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void DeleteProject(const DatahubClientPtr& client, const std::string& project)
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
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
    }

    static DatahubClientPtr client;
    static std::map<std::string, ShardWriterPtr> shardWriterMap;
    static ShardWriterSelectStrategyPtr shardSelector;
    static MessageWriterPtr messageWriter;
};

DatahubClientPtr ShardWriterGTest::client;
std::map<std::string, ShardWriterPtr> ShardWriterGTest::shardWriterMap;
ShardWriterSelectStrategyPtr ShardWriterGTest::shardSelector;
MessageWriterPtr ShardWriterGTest::messageWriter;

TEST_F(ShardWriterGTest, WriteTest)
{
    RecordEntryVec records;
    for (size_t i = 0; i < RECORD_NUM; i++)
    {
        RecordEntry record(BLOB);
        record.SetData("test_blob_" + std::to_string(i));
        records.push_back(record);
    }
    for (auto it = shardWriterMap.begin(); it != shardWriterMap.end(); it++)
    {
        auto writer = it->second;
        for (size_t i = 0; i < EPOCH; i++)
        {
            writer->Write(records);
        }
    }
    sleep(1);
    const auto recordNumForEachShard = ReadRecordEachShard(client, conf_project, conf_topic);
    for (auto it = recordNumForEachShard.begin(); it != recordNumForEachShard.end(); it++)
    {
        ASSERT_EQ(it->second, EPOCH * RECORD_NUM);
    }
}

TEST_F(ShardWriterGTest, WriteAsyncWithoutFlushTest)
{
    RecordEntryVec records;
    for (size_t i = 0; i < RECORD_NUM; i++)
    {
        RecordEntry record(BLOB);
        record.SetData("test_blob_" + std::to_string(i));
        records.push_back(record);
    }
    for (auto it = shardWriterMap.begin(); it != shardWriterMap.end(); it++)
    {
        auto writer = it->second;
        for (size_t i = 0; i < EPOCH; i++)
        {
           writer->WriteAsync(records);
        }
    }
    const auto recordNumForEachShard = ReadRecordEachShard(client, conf_project, conf_topic);
    for (auto it = recordNumForEachShard.begin(); it != recordNumForEachShard.end(); it++)
    {
        ASSERT_EQ(it->second, 0);
    }
}