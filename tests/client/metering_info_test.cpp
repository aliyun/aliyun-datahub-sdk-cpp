#include <memory>
#include <exception>
#include <stdlib.h>
#include "gtest/gtest.h"
#include "datahub/datahub_client.h"
#include "config.h"
#include "utils.h"
#include <iostream>

using namespace aliyun;
using namespace aliyun::datahub;

static const std::string METERING_INFO_PROJECT = "test_metering_info_project";
static const std::string METERING_INFO_TOPIC = "test_metering_info_topic";
static const int SHARDCOUNT = 3;
static const int LIFECYCLE = 7;

class MeteringInfoGTest : public testing::Test
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
            CreateProject();
            CreateTopic();
            PutRecord();
        }
        catch (std::exception& e)
        {
        }
    }

    virtual void TearDown()
    {
        delete client;
    }

    static void CreateProject()
    {
        try
        {
            client->GetProject(METERING_INFO_PROJECT);
        }
        catch (std::exception& e)
        {
            EXPECT_NO_THROW(client->CreateProject(METERING_INFO_PROJECT, "project for metering gtest"));
        }
    }

    static void CreateTopic()
    {
        const std::string& fieldName = "test";
        RecordSchema schema;
        schema.AddField(Field(fieldName, BIGINT));

        const std::string comment = "test";
        RecordType type = TUPLE;
        try
        {
            client->GetTopic(METERING_INFO_PROJECT, METERING_INFO_TOPIC);
        }
        catch (std::exception& e)
        {
            EXPECT_NO_THROW(
                    client->CreateTopic(METERING_INFO_PROJECT, METERING_INFO_TOPIC, SHARDCOUNT, LIFECYCLE, type, schema,
                                        comment));
            EXPECT_NO_THROW(client->WaitForShardReady(METERING_INFO_PROJECT, METERING_INFO_TOPIC));
        }
    }

    static void PutRecord()
    {
        try
        {
            GetTopicResult getTopicResult = client->GetTopic(METERING_INFO_PROJECT, METERING_INFO_TOPIC);
            RecordSchema schema = getTopicResult.GetRecordSchema();
            std::vector<RecordEntry> recordEntries;

            ListShardResult lsr = client->ListShard(METERING_INFO_PROJECT, METERING_INFO_TOPIC);
            std::vector<ShardEntry> shards = lsr.GetShards();
            for (size_t i = 0; i < shards.size(); ++i)
            {
                RecordEntry entry(schema.GetFieldCount());
                entry.SetBigint(schema.GetFieldIndex("test"), rand());
                entry.SetShardId(shards[i].GetShardId());
                entry.SetAttribute("partition", "ds=2019");
                recordEntries.push_back(entry);
            }

            PutRecordResult putRecordResult = client->PutRecord(METERING_INFO_PROJECT, METERING_INFO_TOPIC,
                                                                recordEntries);
        }
        catch (...)
        {

        }
    }

    static DatahubClient* client;
};


DatahubClient* MeteringInfoGTest::client;

TEST_F(MeteringInfoGTest, GetMeteringInfoTest)
{
    ListShardResult lsr = client->ListShard(METERING_INFO_PROJECT, METERING_INFO_TOPIC);
    std::vector<ShardEntry> shards = lsr.GetShards();

    std::string shardId = shards[0].GetShardId();
    std::cout << "ShardId:" << shardId << std::endl;

    try
    {
        GetMeteringInfoResult gmir = client->GetMeteringInfo(METERING_INFO_PROJECT, METERING_INFO_TOPIC, shardId);
        std::cout << "ActiveTime:" << gmir.GetActiveTime() << std::endl;
        std::cout << "Storage:" << gmir.GetStorage() << std::endl;

        ASSERT_EQ(gmir.GetActiveTime() >= 0, true);
        ASSERT_EQ(gmir.GetStorage() >= 0, true);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error:" << e.what() << std::endl;
    }
}

TEST_F(MeteringInfoGTest, GetTopicMeteringInfoTest)
{
    time_t currTime = time(NULL);
    std::string day = Utils::GetDayFromTimeStamp(currTime);
    std::cout << "Day:" << day << std::endl;

    int64_t startTime = Utils::GetStartTimeOfDay(currTime);
    int64_t endTime = Utils::GetEndTimeOfDay(currTime);

    GetTopicMeteringInfoResult gtmir = client->GetTopicMeteringInfo(METERING_INFO_PROJECT, METERING_INFO_TOPIC, day);
    const std::vector<MeterRecord>& meterData = gtmir.GetMeterData();
    std::cout << "size:" << meterData.size() << std::endl;
    ASSERT_EQ(meterData.size() >= 0, true);

    for (size_t i = 0; i < meterData.size(); ++i)
    {
        MeterRecord r = meterData[i];
        ASSERT_EQ(r.GetActiveTime() >= 0, true);
        ASSERT_EQ(r.GetStorageSize() >= 0, true);
        ASSERT_EQ(r.GetReadDataSize() >= 0, true);
        ASSERT_EQ(r.GetWriteDataSize() >= 0, true);
        ASSERT_EQ(r.GetReadTimes() >= 0, true);
        ASSERT_EQ(r.GetWriteTimes() >= 0, true);
        ASSERT_EQ(r.GetConnectorDataSize() >= 0, true);
        ASSERT_EQ(r.GetStartTime() >= startTime, true);
        ASSERT_EQ(r.GetEndTime() <= endTime, true);
    }
}