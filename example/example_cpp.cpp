#include <unistd.h>
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <stdio.h>
#include "datahub/datahub_client.h"

using namespace aliyun;
using namespace aliyun::datahub;

int main(int argc, char *argv[])
{
    /* Configuration */
    Account account;
    account.id = "";
    account.key = "=";

    std::string projectName = "test_project";
    std::string topicName = "test_cpp";
    std::string comment = "test";

    std::string endpoint = "";
    Configuration conf(account, endpoint);

    /* Datahub Client */
    DatahubClient client(conf);

    /****** Project ******/
    /* Create Project */
    try
    {
        client.CreateProject(projectName, comment);
    }
    catch (std::exception& e)
    {
    }

    /* Get Project */
    const GetProjectResult& projectResult = client.GetProject(projectName);
    std::cout<<projectResult.GetProject()<<std::endl;
    std::cout<<projectResult.GetComment()<<std::endl;
    std::cout<<projectResult.GetCreator()<<std::endl;
    std::cout<<projectResult.GetCreateTime()<<std::endl;
    std::cout<<projectResult.GetLastModifyTime()<<std::endl;

    std::string updateProjectComment = "comment_test_1";
    client.UpdateProject(projectName, updateProjectComment);

    sleep(1);

    const GetProjectResult& projectResult4Update = client.GetProject(projectName);
    std::cout<<projectResult4Update.GetComment()<<std::endl;
    std::cout<<projectResult4Update.GetLastModifyTime()<<std::endl;

    const ListProjectResult& listProjectResult = client.ListProject();
    std::cout<<listProjectResult.GetProjectNames().size()<<std::endl;

    /****** Topic ******/
    /* Topic Schema */
    RecordSchema schema;
    std::string fieldName1 = "a";
    std::string fieldName2 = "b";
    std::string fieldName3 = "c";
    std::string fieldComment1 = "field1 comment";
    std::string fieldComment2 = "field2 comment";
    std::string fieldComment3 = "field3 comment";
    schema.AddField(Field(fieldName1/*Fieldname*/, BIGINT, true, fieldComment1));
    schema.AddField(Field(fieldName2/*Fieldname*/, DOUBLE, true, fieldComment2));
    schema.AddField(Field(fieldName3/*Fieldname*/, STRING, true, fieldComment3));

    /* Create Topic */
    int shardCount = 3;
    int lifeCycle = 7;
    RecordType type = TUPLE;
    try
    {
        client.CreateTopic(projectName, topicName, shardCount, lifeCycle, type, schema, comment);
    }
    catch (std::exception& e)
    {
    }

    client.WaitForShardReady(projectName, topicName);

    sleep(5);
    const std::string updateComment = "test1";
    int updateLifecycle = 7;
    client.UpdateTopic(projectName, topicName, updateLifecycle, updateProjectComment);

    const GetTopicResult& getTopicResult = client.GetTopic(projectName, topicName);
    std::cout<<getTopicResult.GetComment()<<std::endl;

    const ListTopicResult& listTopicResult = client.ListTopic(projectName);
    std::cout<<listTopicResult.GetTopicNames().size()<<std::endl;

    /****** Shard ******/
    sleep(5);
    const SplitShardResult& ssr = client.SplitShard(projectName, topicName, "0", "00000000000000000000000000AAAAAA");
    std::cout<<ssr.GetChildShards().size()<<std::endl;

    const ListShardResult& lsr = client.ListShard(projectName, topicName);
    std::cout<<lsr.GetShards().size()<<std::endl;
    std::vector<ShardEntry> shards = lsr.GetShards();

    std::vector<std::string> shardIds;
    for (size_t i = 0; i < shards.size(); ++i)
    {
        ShardEntry shardEntry = shards[i];
        shardIds.push_back(shardEntry.GetShardId());
    }

    std::string beginKey;
    std::string endKey;
    for (size_t i = 0; i < shards.size(); ++i)
    {
        if (shards[i].GetShardId() == "0")
        {
            beginKey = shards[i].GetBeginHashKey();
        }
        if (shards[i].GetShardId() == "1")
        {
            endKey = shards[i].GetEndHashKey();
        }
    }
    sleep(5);
    const MergeShardResult& msr = client.MergeShard(projectName, topicName, "0", "1");
    std::cout<<msr.GetChildShard().GetShardId()<<std::endl;
    std::cout<<msr.GetChildShard().GetBeginHashKey()<<std::endl;
    std::cout<<msr.GetChildShard().GetEndHashKey()<<std::endl;

    /****** Record ******/
    /* Build Record */
    std::vector<RecordEntry> records;
    for (int32_t i = 0; i < 100; ++i)
    {
        RecordEntry record(schema.GetFieldCount());
        record.SetShardId("0");
        for (int j = 0; j < schema.GetFieldCount(); ++j)
        {
            const Field& field = schema.GetField(j);
            const FieldType fieldType = field.GetFieldType();
            switch(fieldType)
            {
                case BIGINT:
                    record.SetBigint(j, 1);
                    break;
                case DOUBLE:
                    record.SetDouble(j, 117.120799999999);
                    break;
                case STRING:
                    record.SetString(j, "345");
                    break;
                default:
                    break;
            }
        }
        records.push_back(record);
    }

    /* Put Record */
    const PutRecordResult& put_ret0 = client.PutRecord(projectName, topicName, records);
    std::cout<<put_ret0.GetFailedRecordCount()<<std::endl;

    const PutRecordResult& put_ret1 = client.PutRecord(projectName, topicName, records);
    std::cout<<put_ret1.GetFailedRecordCount()<<std::endl;

    const PutRecordResult& put_ret2 = client.PutRecord(projectName, topicName, records);
    std::cout<<put_ret2.GetFailedRecordCount()<<std::endl;

    sleep(1);
    /* Get Cursor */
    const GetCursorResult& r1 = client.GetCursor(projectName, topicName, "0", CURSOR_TYPE_OLDEST);

    std::string cursor = r1.GetCursor();

    /* Get Record */
    const GetRecordResult& r2 = client.GetRecord(projectName, topicName, "0", cursor, 1000);
    int count = r2.GetRecordCount();
    std::cout << "read record count is: " << count << std::endl;
    for (int i = 0; i < count; ++i)
    {
        const RecordResult& recordResult = r2.GetRecord(i);
        for (int j = 0; j < schema.GetFieldCount(); ++j)
        {
            const Field& field = schema.GetField(j);
            const FieldType fieldType = field.GetFieldType();
            switch(fieldType)
            {
                case BIGINT:
                    std::cout << recordResult.GetBigint(j) << std::endl;
                    break;
                case DOUBLE:
                    printf("%.15lf\n", recordResult.GetDouble(j));
                    break;
                case STRING:
                    std::cout << recordResult.GetString(j) << std::endl;
                    break;
                default:
                    break;
            }
        }
    }

    /****** Subscription ******/
    const std::string subscriptionComment = "test_subscription";
    const CreateSubscriptionResult& createSubscriptionResult = client.CreateSubscription(projectName, topicName, subscriptionComment);

    const ListSubscriptionResult& subscriptionResult = client.ListSubscription(projectName, topicName, 1, 20, createSubscriptionResult.GetSubId());
    std::cout << subscriptionResult.GetTotalCount() << std::endl;

    const std::vector<SubscriptionEntry>& subscriptions = subscriptionResult.GetSubscriptions();
    std::cout << subscriptions.size() << std::endl;

    for (size_t i = 0; i < subscriptions.size(); ++i)
    {
        SubscriptionEntry entry = subscriptions[i];

        const std::string updateSubscriptionComment = "test_subscription_1";
        client.UpdateSubscription(projectName, topicName, entry.GetSubId(), updateSubscriptionComment);

        const GetSubscriptionResult& getSubscriptionResult = client.GetSubscription(projectName, topicName, entry.GetSubId());
        std::cout << getSubscriptionResult.GetComment() << std::endl;

        client.UpdateSubscriptionState(projectName, topicName, entry.GetSubId(), SubscriptionState::OFFLINE);
        const GetSubscriptionResult& getSubscriptionResult1 = client.GetSubscription(projectName, topicName, entry.GetSubId());
        std::cout << getSubscriptionResult1.GetState() << std::endl;

        client.DeleteSubscription(projectName, topicName, entry.GetSubId());
    }

    /****** Offset ******/
    // Init
    const OpenSubscriptionOffsetSessionResult& offsetSessionResult =
            client.InitSubscriptionOffsetSession(projectName, topicName, createSubscriptionResult.GetSubId(), shardIds);
    std::cout << offsetSessionResult.GetOffsets().size() << std::endl;

    const std::map<std::string, SubscriptionOffset>& offsets = offsetSessionResult.GetOffsets();
    for (auto iter = offsets.begin(); iter != offsets.end(); ++iter)
    {
        const SubscriptionOffset& sowv = iter->second;

        std::cout << sowv.GetTimestamp() << std::endl;
        std::cout << sowv.GetSequence() << std::endl;
        std::cout << sowv.GetVersion() << std::endl;
        std::cout << sowv.GetSessionId() << std::endl;
    }

    // Get
    const GetSubscriptionOffsetResult& getSubscriptionOffsetResult =
            client.GetSubscriptionOffset(projectName, topicName, createSubscriptionResult.GetSubId(), shardIds);
    std::cout << getSubscriptionOffsetResult.GetOffsets().size() << std::endl;

    // Update
    int64_t updateTimestamp = 1l;
    int64_t updateSequence = 1l;
    std::map<std::string, SubscriptionOffset> updateSubscriptionOffsets;
    for (auto iter = offsets.begin(); iter != offsets.end(); ++iter)
    {
        SubscriptionOffset sows = iter->second;

        sows.SetTimestamp(updateTimestamp);
        sows.SetSequence(updateSequence);

        updateSubscriptionOffsets.insert(
                std::pair<std::string, SubscriptionOffset>(iter->first, sows));
    }

    client.UpdateSubscriptionOffset(projectName, topicName, createSubscriptionResult.GetSubId(), updateSubscriptionOffsets);

    const GetSubscriptionOffsetResult& updatedSubscriptionOffsetResult =
            client.GetSubscriptionOffset(projectName, topicName, createSubscriptionResult.GetSubId(), shardIds);
    std::cout << updatedSubscriptionOffsetResult.GetOffsets().size() << std::endl;

    const std::map<std::string, SubscriptionOffset>& updatedOffsets = updatedSubscriptionOffsetResult.GetOffsets();
    for (auto updatedIter = updatedOffsets.begin(); updatedIter != updatedOffsets.end(); ++updatedIter)
    {
        const SubscriptionOffset& sowv = updatedIter->second;

        std::cout << sowv.GetTimestamp() << std::endl;
        std::cout << sowv.GetSequence() << std::endl;
        std::cout << sowv.GetBatchIndex() << std::endl;
    }

    //Reset
    int64_t resetTimestamp = 0l;
    int64_t resetSequence = 0l;
    uint32_t resetBatchIndex = 0u;

    std::map<std::string, SubscriptionOffset> resetSubscriptionOffsets;
    for (auto iter = offsets.begin(); iter != offsets.end(); ++iter)
    {
        SubscriptionOffset offset(resetTimestamp, resetSequence, resetBatchIndex);

        resetSubscriptionOffsets.insert(
                std::pair<std::string, SubscriptionOffset>(iter->first, offset));
    }

    client.ResetSubscriptionOffset(projectName, topicName, createSubscriptionResult.GetSubId(), resetSubscriptionOffsets);

    const GetSubscriptionOffsetResult& resetSubscriptionOffsetResult =
            client.GetSubscriptionOffset(projectName, topicName, createSubscriptionResult.GetSubId(), shardIds);
    std::cout << resetSubscriptionOffsetResult.GetOffsets().size() << std::endl;

    const std::map<std::string, SubscriptionOffset>& resetOffsets = resetSubscriptionOffsetResult.GetOffsets();
    for (auto resetIter = resetOffsets.begin(); resetIter != resetOffsets.end(); ++resetIter)
    {
        const SubscriptionOffset& sowv = resetIter->second;

        std::cout << sowv.GetTimestamp() << std::endl;
        std::cout << sowv.GetSequence() << std::endl;
        std::cout << sowv.GetBatchIndex() << std::endl;
    }

    /****** Connector ******/
    sdk::SinkOdpsConfig config;
    config.SetEndpoint("ODPS_ENDPOINT");
    config.SetProject("ODPS_PROJECT");
    config.SetTable("ODPS_TABLE");
    config.SetAccessId("ODPS_ACCESSID");
    config.SetAccessKey("ODPS_ACCESSKEY");
    config.SetPartitionMode(sdk::PartitionMode::SYSTEM_TIME);
    config.SetTimeRange(15);

    std::vector<std::pair<std::string, std::string> > partitionConfig;
    partitionConfig.push_back(std::pair<std::string, std::string>("ds", "%Y%m%d"));
    partitionConfig.push_back(std::pair<std::string, std::string>("hh", "%H"));
    partitionConfig.push_back(std::pair<std::string, std::string>("mm", "%M"));
    config.SetPartitionConfig(partitionConfig);

    std::vector<std::string> columnFields;
    columnFields.push_back(fieldName1);

    const CreateConnectorResult& connectorResult = client.CreateConnector(projectName, topicName, sdk::ConnectorType::SINK_ODPS, columnFields, config);

    std::cout<<"Odps ConnectorId:" + connectorResult.GetConnectorId()<<std::endl;

    const ListConnectorResult& listConnectorResult = client.ListConnector(projectName, topicName);
    std::cout << listConnectorResult.GetConnectorIds().size() << std::endl;

    std::string connectorId = listConnectorResult.GetConnectorIds()[0];

    const GetConnectorResult& getConnectorResult = client.GetConnector(projectName, topicName, connectorId);

    const sdk::SinkOdpsConfig* odpsConfig = dynamic_cast<const sdk::SinkOdpsConfig*>(getConnectorResult.GetConfig());
    std::cout << odpsConfig->GetPartitionConfig().size() << std::endl;

    const GetConnectorShardStatusResult& shardStatusResult = client.GetConnectorShardStatus(projectName, topicName, connectorId);
    std::map<std::string, sdk::ConnectorShardStatusEntry> statusEntryMap = shardStatusResult.GetStatusEntryMap();
    std::cout << statusEntryMap.size() << std::endl;

    const GetConnectorShardStatusByShardResult& shardStatusByShardResult = client.GetConnectorShardStatusByShard(projectName, topicName, connectorId, "0");
    sdk::ConnectorShardStatusEntry statusEntry = shardStatusByShardResult.GetStatusEntry();
    std::cout << (statusEntry.GetState()>= sdk::ConnectorShardState::CONTEXT_HANG && statusEntry.GetState() <= sdk::ConnectorShardState::CONTEXT_FINISHED) << std::endl;

    const GetConnectorDoneTimeResult& doneTimeResult = client.GetConnectorDoneTime(projectName, topicName, connectorId);
    std::cout << doneTimeResult.GetDoneTime() << std::endl;

    client.ReloadConnector(projectName, topicName, connectorId, "0");

    client.ReloadConnector(projectName, topicName, connectorId);

    client.UpdateConnectorState(projectName, topicName, connectorId, sdk::ConnectorState::CONNECTOR_STOPPED);

    const GetConnectorResult& updateStateGetConnectorResult = client.GetConnector(projectName, topicName, connectorId);
    std::cout << (updateStateGetConnectorResult.GetState() == sdk::ConnectorState::CONNECTOR_STOPPED) << std::endl;

    sdk::ConnectorOffset connectorOffset(9, 1000);
    client.UpdateConnectorOffsetByShard(projectName, topicName, connectorId, "0", connectorOffset);

    const GetConnectorShardStatusByShardResult& updateOffsetShardStatusByShardResult = client.GetConnectorShardStatusByShard(projectName, topicName, connectorId, "0");
    const sdk::ConnectorShardStatusEntry& updateOffsetShardStatusEntry = updateOffsetShardStatusByShardResult.GetStatusEntry();
    std::cout << (updateOffsetShardStatusEntry.GetCurrentTimestamp() == 9) << std::endl;
    std::cout << (updateOffsetShardStatusEntry.GetCurrentSequence() == 1000) << std::endl;

    sdk::ConnectorOffset connectorOffset1(10, 1001);
    client.UpdateConnectorOffset(projectName, topicName, connectorId, connectorOffset1);

    // const std::string& appendFieldType = GetNameForFieldType(BIGINT);
    // client.AppendField(projectName, topicName, fieldName2, appendFieldType);
    client.AppendConnectorField(projectName, topicName, connectorId, fieldName2);

    const GetConnectorResult& appendFieldConnectorResult = client.GetConnector(projectName, topicName, connectorId);
    std::vector<std::string> appendColumnFields = appendFieldConnectorResult.GetColumnFields();
    std::cout << (appendColumnFields.size() == 2) << std::endl;

    sdk::SinkOdpsConfig updateConfig;
    updateConfig.SetEndpoint("ODPS_ENDPOINT");
    updateConfig.SetProject("ODPS_PROJECT");
    updateConfig.SetTable("ODPS_TABLE");
    updateConfig.SetAccessId("ODPS_ACCESSID");
    updateConfig.SetAccessKey("ODPS_ACCESSKEY");
    updateConfig.SetPartitionMode(sdk::PartitionMode::EVENT_TIME);
    updateConfig.SetTimeRange(30);

    std::vector<std::pair<std::string, std::string> > updatePartitionConfig;
    updatePartitionConfig.push_back(std::pair<std::string, std::string>("ds", "%Y%m%d"));
    updatePartitionConfig.push_back(std::pair<std::string, std::string>("hh", "%H"));
    updateConfig.SetPartitionConfig(updatePartitionConfig);

    client.UpdateConnector(projectName, topicName, connectorId, updateConfig);

    const GetConnectorResult& updateConnectorResult = client.GetConnector(projectName, topicName, connectorId);

    const sdk::SinkOdpsConfig* updateOdpsConfig = dynamic_cast<const sdk::SinkOdpsConfig*>(updateConnectorResult.GetConfig());
    std::cout << (updateOdpsConfig->GetPartitionMode() == sdk::PartitionMode::EVENT_TIME) << std::endl;
    std::cout << (updateOdpsConfig->GetTimeRange() == 30) << std::endl;
    std::cout << (updateOdpsConfig->GetPartitionConfig().size() == 2ul) << std::endl;

    client.DeleteConnector(projectName, topicName, connectorId);

    /****** SchemaRegister ******/
    RecordSchema recordSchema;
    recordSchema.AddField(Field("string_field", STRING));
    recordSchema.AddField(Field("double_field", DOUBLE));

    const RegisterTopicSchemaResult& registerSchema = client.RegisterTopicSchema(projectName, topicName, recordSchema);
    std::cout << registerSchema.GetVersionId() << std::endl;

    const ListTopicSchemaResult& listSchema = client.ListTopicSchema(projectName, topicName);
    std::cout << listSchema.GetTotalCount() << std::endl;
    std::cout << listSchema.GetPageCount() << std::endl;
    std::cout << listSchema.GetPageNumber() << std::endl;
    std::cout << listSchema.GetPageSize() << std::endl;

    std::vector<RecordSchema> schemaVec = listSchema.GetRecordSchemaList();
    for (auto it = schemaVec.begin(); it != schemaVec.end(); it++)
    {
        std::cout << it->ToJsonString() << std::endl;
    }

    const GetTopicSchemaResult& getVersionFromSchema = client.GetTopicSchema(projectName, topicName, recordSchema);
    std::cout << getVersionFromSchema.GetVersionId() << std::endl;
    std::cout << getVersionFromSchema.GetRecordSchema().ToJsonString() << std::endl;

    const GetTopicSchemaResult& getSchemaFromVersion = client.GetTopicSchema(projectName, topicName, getVersionFromSchema.GetVersionId());
    std::cout << getSchemaFromVersion.GetVersionId() << std::endl;
    std::cout << getSchemaFromVersion.GetRecordSchema().ToJsonString() << std::endl;

    client.DeleteTopicSchema(projectName, topicName, registerSchema.GetVersionId());

    // Clean Topic & Project
    try
    {
        client.DeleteTopic(projectName, topicName);
    }
    catch (std::exception& e)
    {
    }

    try
    {
        client.DeleteProject(projectName);
    }
    catch (std::exception& e)
    {
    }
    return 0;
}
