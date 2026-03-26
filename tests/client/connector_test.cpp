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

static const std::string ODPS_ENDPOINT = "http://service.odpsstg.aliyun-inc.com/stgnew";
static const std::string ODPS_PROJECT = "alifin_jtest_dev";
static const std::string ODPS_TABLE = "test_datahub_sink_odps";
static const std::string ODPS_ACCESSID = "";
static const std::string ODPS_ACCESSKEY = "";

static const std::string ADS_HOST = "ads-test-7a527933.ads-daily.ads.aliyuncs.com";
static const std::string ADS_PORT = "10006";
static const std::string ADS_DATABASE = "ads_test";
static const std::string ADS_TABLE_GROUP = "datahub_dailyrun";
static const std::string ADS_TABLE = "test_datahub_ads_connector";
static const std::string ADS_USER = "";
static const std::string ADS_PASSWORD = "";

static const std::string MYSQL_HOST = "10.101.214.153";
static const std::string MYSQL_PORT = "3306";
static const std::string MYSQL_DATABASE = "datahub_sdk_e2e_test";
static const std::string MYSQL_TABLE = "test_datahub_mysql_connector";
static const std::string MYSQL_USER = "apsara";
static const std::string MYSQL_PASSWORD = "123456";

static const std::string OSS_ENDPOINT = "http://cn-hangzhou.oss.aliyun-inc.com";
static const std::string OSS_BUCKET = "100839";
static const std::string OSS_PREFIX = "sink_oss";
static const std::string OSS_ACCESSID = "";
static const std::string OSS_ACCESSKEY = "";

static const std::string OTS_ENDPOINT = "http://test-dh-sink.cn-hangzhou.ots.aliyuncs.com";
static const std::string OTS_INSTANCE = "test-dh-sink";
// tt2datahub@aliyun-inner.com SubAccount
static const std::string OTS_TABLE  = "datahub_sink_ots_tbl";
static const std::string OTS_ACCESSID = "";
static const std::string OTS_ACCESSKEY = "";

static const std::string FC_ENDPOINT = "http://1304576691199657.fc.cn-shanghai.aliyuncs.com";
static const std::string FC_SERVICE = "sink_fc_test";
static const std::string FC_FUNCTION = "test_function";
static const std::string FC_ACCESSID = "";
static const std::string FC_ACCESSKEY = "";

static const std::string DATAHUB_ENDPOINT = "http://10.101.200.225:8992";
static const std::string DATAHUB_PROJECT = "new_sdk_e2e_test";
static const std::string DATAHUB_ACCESSID = "";
static const std::string DATAHUB_ACCESSKEY = "";

class ConnectorGTest: public testing::Test
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

DatahubClient* ConnectorGTest::client;

/**
 * CREATE TABLE IF NOT EXISTS test_datahub_sink_odps(test BIGINT, test1 STRING) PARTITIONED BY (ds STRING,hh STRING,mm STRING);
 */
TEST_F(ConnectorGTest, OdpsConnectorTest)
{
    const std::string& fieldName = "test";
    const std::string& fieldName1 = "test1";
    RecordSchema schema;
    schema.AddField(Field(fieldName, BIGINT));
    schema.AddField(Field(fieldName1, STRING));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    sdk::SinkOdpsConfig config;
    config.SetEndpoint(ODPS_ENDPOINT);
    config.SetProject(ODPS_PROJECT);
    config.SetTable(ODPS_TABLE);
    config.SetAccessId(ODPS_ACCESSID);
    config.SetAccessKey(ODPS_ACCESSKEY);
    config.SetPartitionMode(sdk::PartitionMode::SYSTEM_TIME);
    config.SetTimeRange(15);

    std::vector<std::pair<std::string, std::string> > partitionConfig;
    partitionConfig.push_back(std::pair<std::string, std::string>("ds", "%Y%m%d"));
    partitionConfig.push_back(std::pair<std::string, std::string>("hh", "%H"));
    partitionConfig.push_back(std::pair<std::string, std::string>("mm", "%M"));
    config.SetPartitionConfig(partitionConfig);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_ODPS, columnFields, config);

    std::cout<<"Odps ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId1 = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_ODPS);

    const sdk::SinkOdpsConfig* odpsConfig = dynamic_cast<const sdk::SinkOdpsConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, odpsConfig->GetPartitionMode() == sdk::PartitionMode::SYSTEM_TIME);
    ASSERT_EQ(true, odpsConfig->GetTimeRange() == 15);
    ASSERT_EQ(3ul, odpsConfig->GetPartitionConfig().size());

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    GetConnectorDoneTimeResult doneTimeResult = client->GetConnectorDoneTime(conf_project, topic, connectorId);
    ASSERT_EQ(true, doneTimeResult.GetDoneTime() >= 0);

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    client->AppendConnectorField(conf_project, topic, connectorId, fieldName1);

    GetConnectorResult appendFieldConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    std::vector<std::string> appendColumnFields = appendFieldConnectorResult.GetColumnFields();
    ASSERT_EQ(true, appendColumnFields.size() == 2);

    sdk::SinkOdpsConfig updateConfig;
    updateConfig.SetEndpoint(ODPS_ENDPOINT);
    updateConfig.SetProject(ODPS_PROJECT);
    updateConfig.SetTable(ODPS_TABLE);
    updateConfig.SetAccessId(ODPS_ACCESSID);
    updateConfig.SetAccessKey(ODPS_ACCESSKEY);
    updateConfig.SetPartitionMode(sdk::PartitionMode::EVENT_TIME);
    updateConfig.SetTimeRange(30);

    std::vector<std::pair<std::string, std::string> > updatePartitionConfig;
    updatePartitionConfig.push_back(std::pair<std::string, std::string>("ds", "%Y%m%d"));
    updatePartitionConfig.push_back(std::pair<std::string, std::string>("hh", "%H"));
    updateConfig.SetPartitionConfig(updatePartitionConfig);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);

    const sdk::SinkOdpsConfig* updateOdpsConfig = dynamic_cast<const sdk::SinkOdpsConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateOdpsConfig->GetPartitionMode() == sdk::PartitionMode::EVENT_TIME);
    ASSERT_EQ(true, updateOdpsConfig->GetTimeRange() == 30);
    ASSERT_EQ(2ul, updateOdpsConfig->GetPartitionConfig().size());

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}

TEST_F(ConnectorGTest, DatahubConnectorTest)
{
    const std::string& fieldName = "test";
    const std::string& fieldName1 = "test1";
    RecordSchema schema;
    schema.AddField(Field(fieldName, BIGINT));
    schema.AddField(Field(fieldName1, STRING));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    sdk::SinkDatahubConfig config;
    config.SetEndpoint(conf_endpoint);
    config.SetProject(conf_project);
    config.SetTopic(topic);
    config.SetAuthMode(sdk::AuthMode::AK);
    config.SetAccessId(conf_accessId);
    config.SetAccessKey(conf_accessKey);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);
    columnFields.push_back(fieldName1);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_DATAHUB, columnFields, config);

    std::cout<<"Datahub ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId1 = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_DATAHUB);

    const sdk::SinkDatahubConfig* datahubConfig = dynamic_cast<const sdk::SinkDatahubConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, datahubConfig->GetEndpoint() == conf_endpoint);
    ASSERT_EQ(true, datahubConfig->GetProject() == conf_project);
    ASSERT_EQ(true, datahubConfig->GetTopic() == topic);
    ASSERT_EQ(true, datahubConfig->GetAuthMode() == sdk::AuthMode::AK);

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    const std::string& appendFieldName = "test2";
    const std::string& appendFieldType = GetNameForFieldType(BIGINT);
    client->AppendField(conf_project, topic, appendFieldName, appendFieldType);
    client->AppendConnectorField(conf_project, topic, connectorId, appendFieldName);

    GetConnectorResult appendFieldConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    std::vector<std::string> appendColumnFields = appendFieldConnectorResult.GetColumnFields();
    ASSERT_EQ(true, appendColumnFields.size() == 3);

    sdk::SinkDatahubConfig updateConfig;
    updateConfig.SetEndpoint(DATAHUB_ENDPOINT);
    updateConfig.SetProject(DATAHUB_PROJECT);
    updateConfig.SetTopic(topic);
    updateConfig.SetAuthMode(sdk::AuthMode::AK);
    updateConfig.SetAccessId(DATAHUB_ACCESSID);
    updateConfig.SetAccessKey(DATAHUB_ACCESSKEY);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    const sdk::SinkDatahubConfig* updateDatahubConfig = dynamic_cast<const sdk::SinkDatahubConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateDatahubConfig->GetEndpoint() == DATAHUB_ENDPOINT);
    ASSERT_EQ(true, updateDatahubConfig->GetProject() == DATAHUB_PROJECT);
    ASSERT_EQ(true, updateDatahubConfig->GetTopic() == topic);
    ASSERT_EQ(true, updateDatahubConfig->GetAuthMode() == sdk::AuthMode::AK);

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}

TEST_F(ConnectorGTest, FcConnectorTest)
{
    const std::string& fieldName = "test";
    const std::string& fieldName1 = "test1";
    RecordSchema schema;
    schema.AddField(Field(fieldName, BIGINT));
    schema.AddField(Field(fieldName1, STRING));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    sdk::SinkFcConfig config;
    config.SetEndpoint(FC_ENDPOINT);
    config.SetService(FC_SERVICE);
    config.SetFunction(FC_FUNCTION);
    config.SetAuthMode(sdk::AuthMode::AK);
    config.SetAccessId(FC_ACCESSID);
    config.SetAccessKey(FC_ACCESSKEY);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_FC, columnFields, config);

    std::cout<<"Fc ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId1 = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_FC);

    const sdk::SinkFcConfig* fcConfig = dynamic_cast<const sdk::SinkFcConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, fcConfig->GetEndpoint() == FC_ENDPOINT);
    ASSERT_EQ(true, fcConfig->GetService() == FC_SERVICE);
    ASSERT_EQ(true, fcConfig->GetFunction() == FC_FUNCTION);
    ASSERT_EQ(true, fcConfig->GetAuthMode() == sdk::AuthMode::AK);

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    client->AppendConnectorField(conf_project, topic, connectorId, fieldName1);

    GetConnectorResult appendFieldConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    std::vector<std::string> appendColumnFields = appendFieldConnectorResult.GetColumnFields();
    ASSERT_EQ(true, appendColumnFields.size() == 2);

    std::string updateFunction = FC_FUNCTION + "1";
    sdk::SinkFcConfig updateConfig;
    updateConfig.SetEndpoint(FC_ENDPOINT);
    updateConfig.SetService(FC_SERVICE);
    updateConfig.SetFunction(updateFunction);
    updateConfig.SetAuthMode(sdk::AuthMode::AK);
    updateConfig.SetAccessId(FC_ACCESSID);
    updateConfig.SetAccessKey(FC_ACCESSKEY);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    const sdk::SinkFcConfig* updateFcConfig = dynamic_cast<const sdk::SinkFcConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateFcConfig->GetEndpoint() == FC_ENDPOINT);
    ASSERT_EQ(true, updateFcConfig->GetService() == FC_SERVICE);
    ASSERT_EQ(true, updateFcConfig->GetFunction() == updateFunction);
    ASSERT_EQ(true, updateFcConfig->GetAuthMode() == sdk::AuthMode::AK);

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}

TEST_F(ConnectorGTest, OssConnectorTest)
{
    const std::string& fieldName = "test";
    const std::string& fieldName1 = "test1";
    RecordSchema schema;
    schema.AddField(Field(fieldName, BIGINT));
    schema.AddField(Field(fieldName1, STRING));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    std::string timeFormat = "%Y%m%d%H%M";
    sdk::SinkOssConfig config;
    config.SetEndpoint(OSS_ENDPOINT);
    config.SetBucket(OSS_BUCKET);
    config.SetPrefix(OSS_PREFIX);
    config.SetTimeFormat(timeFormat);
    config.SetTimeRange(15);
    config.SetAuthMode(sdk::AuthMode::AK);
    config.SetAccessId(OSS_ACCESSID);
    config.SetAccessKey(OSS_ACCESSKEY);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_OSS, columnFields, config);

    std::cout<<"Oss ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId1 = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_OSS);

    const sdk::SinkOssConfig* ossConfig = dynamic_cast<const sdk::SinkOssConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, ossConfig->GetEndpoint() == OSS_ENDPOINT);
    ASSERT_EQ(true, ossConfig->GetBucket() == OSS_BUCKET);
    ASSERT_EQ(true, ossConfig->GetPrefix() == OSS_PREFIX);
    ASSERT_EQ(true, ossConfig->GetTimeFormat() == timeFormat);
    ASSERT_EQ(true, ossConfig->GetTimeRange() == 15);
    ASSERT_EQ(true, ossConfig->GetAuthMode() == sdk::AuthMode::AK);

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    client->AppendConnectorField(conf_project, topic, connectorId, fieldName1);

    GetConnectorResult appendFieldConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    std::vector<std::string> appendColumnFields = appendFieldConnectorResult.GetColumnFields();
    ASSERT_EQ(true, appendColumnFields.size() == 2);

    std::string updateTimeFormat = "%Y%m%d%H%M";
    sdk::SinkOssConfig updateConfig;
    updateConfig.SetEndpoint(OSS_ENDPOINT);
    updateConfig.SetBucket(OSS_BUCKET);
    updateConfig.SetPrefix(OSS_PREFIX);
    updateConfig.SetTimeFormat(updateTimeFormat);
    updateConfig.SetTimeRange(30);
    updateConfig.SetAuthMode(sdk::AuthMode::AK);
    updateConfig.SetAccessId(OSS_ACCESSID);
    updateConfig.SetAccessKey(OSS_ACCESSKEY);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    const sdk::SinkOssConfig* updateOssConfig = dynamic_cast<const sdk::SinkOssConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateOssConfig->GetEndpoint() == OSS_ENDPOINT);
    ASSERT_EQ(true, updateOssConfig->GetBucket() == OSS_BUCKET);
    ASSERT_EQ(true, updateOssConfig->GetPrefix() == OSS_PREFIX);
    ASSERT_EQ(true, updateOssConfig->GetTimeFormat() == updateTimeFormat);
    ASSERT_EQ(true, updateOssConfig->GetTimeRange() == 30);
    ASSERT_EQ(true, updateOssConfig->GetAuthMode() == sdk::AuthMode::AK);

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}

TEST_F(ConnectorGTest, OtsConnectorTest)
{

    const std::string& fieldName = "pk1";
    const std::string& fieldName1 = "val1";
    const std::string& fieldName2 = "pk2";
    const std::string& fieldName3 = "val2";
    const std::string& fieldName4 = "pk3";
    const std::string& fieldName5 = "val3";
    RecordSchema schema;
    schema.AddField(Field(fieldName, STRING));
    schema.AddField(Field(fieldName1, STRING));
    schema.AddField(Field(fieldName2, STRING));
    schema.AddField(Field(fieldName3, STRING));
    schema.AddField(Field(fieldName4, BIGINT));
    schema.AddField(Field(fieldName5, BIGINT));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    sdk::SinkOtsConfig config;
    config.SetEndpoint(OTS_ENDPOINT);
    config.SetInstance(OTS_INSTANCE);
    config.SetTable(OTS_TABLE);
    config.SetAuthMode(sdk::AuthMode::AK);
    config.SetAccessId(OTS_ACCESSID);
    config.SetAccessKey(OTS_ACCESSKEY);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);
    columnFields.push_back(fieldName1);
    columnFields.push_back(fieldName2);
    columnFields.push_back(fieldName3);
    columnFields.push_back(fieldName4);
    columnFields.push_back(fieldName5);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_OTS, columnFields, config);

    std::cout<<"Oss ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId1 = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_OTS);
    ASSERT_EQ(true, getConnectorResult.GetColumnFields().size() == 6);

    const sdk::SinkOtsConfig* itsConfig = dynamic_cast<const sdk::SinkOtsConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, itsConfig->GetEndpoint() == OTS_ENDPOINT);
    ASSERT_EQ(true, itsConfig->GetInstance() == OTS_INSTANCE);
    ASSERT_EQ(true, itsConfig->GetTable() == OTS_TABLE);
    ASSERT_EQ(true, itsConfig->GetAuthMode() == sdk::AuthMode::AK);

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    const std::string& appendFieldName = "val4";
    const std::string& appendFieldType = GetNameForFieldType(BIGINT);
    client->AppendField(conf_project, topic, appendFieldName, appendFieldType);
    client->AppendConnectorField(conf_project, topic, connectorId, appendFieldName);

    GetConnectorResult appendFieldConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    std::vector<std::string> appendColumnFields = appendFieldConnectorResult.GetColumnFields();
    ASSERT_EQ(true, appendColumnFields.size() == 7);

    const std::string& updateTable = OTS_TABLE + "1";
    sdk::SinkOtsConfig updateConfig;
    updateConfig.SetEndpoint(OTS_ENDPOINT);
    updateConfig.SetInstance(OTS_INSTANCE);
    updateConfig.SetTable(updateTable);
    updateConfig.SetAuthMode(sdk::AuthMode::AK);
    updateConfig.SetAccessId(OTS_ACCESSID);
    updateConfig.SetAccessKey(OTS_ACCESSKEY);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    const sdk::SinkOtsConfig* updateOtsConfig = dynamic_cast<const sdk::SinkOtsConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateOtsConfig->GetEndpoint() == OTS_ENDPOINT);
    ASSERT_EQ(true, updateOtsConfig->GetInstance() == OTS_INSTANCE);
    ASSERT_EQ(true, updateOtsConfig->GetTable() == updateTable);
    ASSERT_EQ(true, updateOtsConfig->GetAuthMode() == sdk::AuthMode::AK);

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}

/**
 * mysql -h10.101.214.153 -P3306 -uapsara  -p123456 datahub_sdk_e2e_test -A --default-character-set=utf8
 * CREATE TABLE IF NOT EXISTS `test_datahub_mysql_connector` (`test` varchar(2048) NOT NULL, `test1` bigint(20) NULL) ENGINE=InnoDB DEFAULT CHARSET=utf8
 */
TEST_F(ConnectorGTest, MysqlConnectorTest)
{
    const std::string& fieldName = "test";
    const std::string& fieldName1 = "test1";
    RecordSchema schema;
    schema.AddField(Field(fieldName, STRING));
    schema.AddField(Field(fieldName1, BIGINT));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    sdk::SinkMysqlConfig config;
    config.SetHost(MYSQL_HOST);
    config.SetPort(std::stoi(MYSQL_PORT));
    config.SetDatabase(MYSQL_DATABASE);
    config.SetTable(MYSQL_TABLE);
    config.SetUser(MYSQL_USER);
    config.SetPassword(MYSQL_PASSWORD);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);
    columnFields.push_back(fieldName1);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_MYSQL, columnFields, config);

    std::cout<<"Mysql ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId1 = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_MYSQL);

    const sdk::SinkMysqlConfig* itsConfig = dynamic_cast<const sdk::SinkMysqlConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, itsConfig->GetHost() == MYSQL_HOST);
    ASSERT_EQ(true, itsConfig->GetPort() == std::stoi(MYSQL_PORT));
    ASSERT_EQ(true, itsConfig->GetDatabase() == MYSQL_DATABASE);
    ASSERT_EQ(true, itsConfig->GetTable() == MYSQL_TABLE);

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    const std::string& updateTable = MYSQL_TABLE + "_1";
    sdk::SinkMysqlConfig updateConfig;
    updateConfig.SetHost(MYSQL_HOST);
    updateConfig.SetPort(std::stoi(MYSQL_PORT));
    updateConfig.SetDatabase(MYSQL_DATABASE);
    updateConfig.SetTable(updateTable);
    updateConfig.SetUser(MYSQL_USER);
    updateConfig.SetPassword(MYSQL_PASSWORD);
    updateConfig.SetInsertMode(sdk::InsertMode::OVERWRITE);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    const sdk::SinkMysqlConfig* updateMysqlConfig = dynamic_cast<const sdk::SinkMysqlConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateMysqlConfig->GetHost() == MYSQL_HOST);
    ASSERT_EQ(true, updateMysqlConfig->GetPort() == std::stoi(MYSQL_PORT));
    ASSERT_EQ(true, updateMysqlConfig->GetDatabase() == MYSQL_DATABASE);
    ASSERT_EQ(true, updateMysqlConfig->GetTable() == updateTable);
    ASSERT_EQ(true, updateMysqlConfig->GetInsertMode() == sdk::InsertMode::OVERWRITE);

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}

/**
 * mysql -hads-test-7a527933.ads-daily.ads.aliyuncs.com -P10006 -uLTAIkHLYuMjVhPsv -pbKzJRmgVQw7YLtCWs8Y9vVWWEpP5qv ads_test -A --default-character-set=utf8
 * CREATE TABLE IF NOT EXISTS `test_datahub_ads_connector`(`test` varchar(2048) NOT NULL, `test1` bigint NULL, PRIMARY KEY(test)) PARTITION BY HASH KEY(test) PARTITION NUM 32 TABLEGROUP datahub_dailyrun options (updateType='realtime');
 */
TEST_F(ConnectorGTest, AdsConnectorTest)
{
    const std::string& fieldName = "test";
    const std::string& fieldName1 = "test1";
    RecordSchema schema;
    schema.AddField(Field(fieldName, STRING));
    schema.AddField(Field(fieldName1, BIGINT));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();

    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    sdk::SinkAdsConfig config;
    config.SetHost(ADS_HOST);
    config.SetPort(std::stoi(ADS_PORT));
    config.SetDatabase(ADS_DATABASE);
    config.SetTable(ADS_TABLE);
    config.SetUser(ADS_USER);
    config.SetPassword(ADS_PASSWORD);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName);
    columnFields.push_back(fieldName1);

    CreateConnectorResult connectorResult = client->CreateConnector(conf_project, topic, sdk::ConnectorType::SINK_ADS, columnFields, config);

    std::cout<<"Ads ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    std::string connectorId = connectorResult.GetConnectorId();

    ListConnectorResult listConnectorResult = client->ListConnector(conf_project, topic);
    ASSERT_EQ(1ul, listConnectorResult.GetConnectorIds().size());

    std::string connectorId1 = listConnectorResult.GetConnectorIds()[0];
    ASSERT_EQ(true, connectorId1 == connectorId);

    GetConnectorResult getConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, getConnectorResult.GetType() == sdk::ConnectorType::SINK_ADS);

    const sdk::SinkAdsConfig* adsConfig = dynamic_cast<const sdk::SinkAdsConfig*>(getConnectorResult.GetConfig());
    ASSERT_EQ(true, adsConfig->GetHost() == ADS_HOST);
    ASSERT_EQ(true, adsConfig->GetPort() == std::stoi(ADS_PORT));
    ASSERT_EQ(true, adsConfig->GetDatabase() == ADS_DATABASE);
    ASSERT_EQ(true, adsConfig->GetTable() == ADS_TABLE);

    GetConnectorShardStatusResult shardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    ASSERT_EQ(3ul, statusEntryMap.size());

    GetConnectorShardStatusByShardResult shardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED));

    client->ReloadConnector(conf_project, topic, connectorId, "0");

    client->ReloadConnector(conf_project, topic, connectorId);

    client->UpdateConnectorState(conf_project, topic, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    GetConnectorResult updateStateGetConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    ASSERT_EQ(true, updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED);

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client->UpdateConnectorOffsetByShard(conf_project, topic, connectorId, "0", connectorOffset);

    GetConnectorShardStatusByShardResult updateOffsetShardStatusByShardResult = client->GetConnectorShardStatusByShard(conf_project, topic, connectorId, "0");
    sdk::ConnectorShardStatusEntry updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9);
    ASSERT_EQ(true, updateOffsetShardStatusEntry.GetCurrentSequence() == 1000);

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client->UpdateConnectorOffset(conf_project, topic, connectorId, connectorOffset1);

    GetConnectorShardStatusResult updateOffsetShardStatusResult = client->GetConnectorShardStatus(conf_project, topic, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> updateOffsetStatusEntryMap = updateOffsetShardStatusResult.GetStatusEntryMap();
    std::map<std::string, sdk::ConnectorShardStatusEntry>::iterator updateOffsetStatusIter;
    for(updateOffsetStatusIter = updateOffsetStatusEntryMap.begin(); updateOffsetStatusIter != updateOffsetStatusEntryMap.end(); ++updateOffsetStatusIter) {
        sdk::ConnectorShardStatusEntry entry = updateOffsetStatusIter->second;

        ASSERT_EQ(true, entry.GetCurrentTimestamp() == 10);
        ASSERT_EQ(true, entry.GetCurrentSequence() == 1001);
    }

    const std::string& updateTable = ADS_TABLE + "_1";
    sdk::SinkAdsConfig updateConfig;
    updateConfig.SetHost(ADS_HOST);
    updateConfig.SetPort(std::stoi(ADS_PORT));
    updateConfig.SetDatabase(ADS_DATABASE);
    updateConfig.SetTable(updateTable);
    updateConfig.SetUser(ADS_USER);
    updateConfig.SetPassword(ADS_PASSWORD);
    updateConfig.SetInsertMode(sdk::InsertMode::OVERWRITE);

    client->UpdateConnector(conf_project, topic, connectorId, updateConfig);

    GetConnectorResult updateConnectorResult = client->GetConnector(conf_project, topic, connectorId);
    const sdk::SinkAdsConfig* updateAdsConfig = dynamic_cast<const sdk::SinkAdsConfig*>(updateConnectorResult.GetConfig());
    ASSERT_EQ(true, updateAdsConfig->GetHost() == ADS_HOST);
    ASSERT_EQ(true, updateAdsConfig->GetPort() == std::stoi(ADS_PORT));
    ASSERT_EQ(true, updateAdsConfig->GetDatabase() == ADS_DATABASE);
    ASSERT_EQ(true, updateAdsConfig->GetTable() == updateTable);
    ASSERT_EQ(true, updateAdsConfig->GetInsertMode() == sdk::InsertMode::OVERWRITE);

    client->DeleteConnector(conf_project, topic, connectorId);

    client->DeleteTopic(conf_project, topic);
}
