#include "config.h"
#include "timer.h"
#include "client/write_result.h"
#include "shard_coordinator.h"
#include "shard_group_writer.h"
#include "client/producer/producer_config.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"
#include "gtest/gtest.h"
#include <map>
#include <string>
#include <iostream>
#include <sstream>

#define EPOCH 100
#define RECORD_NUM 100

using namespace aliyun;
using namespace aliyun::datahub;

class ShardGroupWriterGTest: public testing::Test
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
        client = new DatahubClient((Configuration)producerConf);

        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        subId = "";
        client->WaitForShardReady(conf_project, conf_topic);

        StringVec shardIds;
        coordinatorPtr = new ShardCoordinator(conf_project, conf_topic, subId, (CommonConfiguration)producerConf);
        shardGroupWriterPtr = new ShardGroupWriter(coordinatorPtr, shardIds, producerConf);
    }

    int ReadRecordTotal(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        ListShardResult lsr = client->ListShard(project, topic);
        auto shards = lsr.GetShards();
        int totalNum = 0, limit = 100;
        for (auto it = shards.begin(); it != shards.end(); it++)
        {
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
        }
        return totalNum;
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

    static void DeleteTopic(DatahubClient* client, const std::string& project, const std::string& topic)
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
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
        delete shardGroupWriterPtr;
        delete coordinatorPtr;
        delete client;
    }

    static std::string subId;
    static DatahubClient* client;
    static ShardCoordinator* coordinatorPtr;
    static ShardGroupWriter* shardGroupWriterPtr;
};

std::string ShardGroupWriterGTest::subId;
DatahubClient* ShardGroupWriterGTest::client;
ShardCoordinator* ShardGroupWriterGTest::coordinatorPtr;
ShardGroupWriter* ShardGroupWriterGTest::shardGroupWriterPtr;

TEST_F(ShardGroupWriterGTest, WriteTest)
{
    RecordEntryVec records;
    for (size_t i = 0; i < RECORD_NUM; i++)
    {
        RecordEntry record(BLOB);
        record.SetData("test_blob_" + std::to_string(i));
        records.push_back(record);
    }
    for (size_t i = 0; i < EPOCH; i++)
    {
        std::string shardId = shardGroupWriterPtr->Write(records);
        ASSERT_FALSE(shardId.empty());
    }
}

TEST_F(ShardGroupWriterGTest, WriteAsyncTest)
{
    std::vector<WriteResultFuturePtr> resultFutureVecs;
    RecordEntryVec records;
    for (size_t i = 0; i < RECORD_NUM; i++)
    {
        RecordEntry record(BLOB);
        record.SetData("test_blob_" + std::to_string(i));
        records.push_back(record);
    }
    for (size_t i = 0; i < EPOCH; i++)
    {
        auto result = shardGroupWriterPtr->WriteAsync(records);
        if (result == nullptr)
        {
            break;
        }
        resultFutureVecs.push_back(result);
    }
    shardGroupWriterPtr->Flush();

    for (auto it = resultFutureVecs.begin(); it != resultFutureVecs.end(); it++)
    {
        WriteResultPtr result = (*it)->get();
        ASSERT_FALSE(result->GetShardId().empty());
    }
    ASSERT_EQ(ReadRecordTotal(client, conf_project, conf_topic), EPOCH * RECORD_NUM);
}

TEST_F(ShardGroupWriterGTest, WriteAsyncWithoutFlushTest)
{
    RecordEntryVec records;
    for (size_t i = 0; i < RECORD_NUM; i++)
    {
        RecordEntry record(BLOB);
        record.SetData("test_blob_" + std::to_string(i));
        records.push_back(record);
    }
    for (size_t i = 0; i < EPOCH; i++)
    {
        shardGroupWriterPtr->WriteAsync(records);
    }

    ASSERT_EQ(ReadRecordTotal(client, conf_project, conf_topic), 0);
}