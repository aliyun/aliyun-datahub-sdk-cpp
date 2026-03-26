#include "datahub/datahub_client.h"
#include "datahub_client_pb_impl.h"
#include "datahub_client_json_impl.h"

namespace aliyun
{
namespace datahub
{

DatahubClient::DatahubClient(const Configuration& conf)
{
    if (conf.GetEnableProtobuf())
    {
        mDatahubClientImpl = new DatahubClientPBImpl(conf);
    }
    else
    {
        mDatahubClientImpl = new DatahubClientJsonImpl(conf);
    }
}

DatahubClient::~DatahubClient()
{
    delete mDatahubClientImpl;
}

ListProjectResult DatahubClient::ListProject()
{
    ListProjectResult result = mDatahubClientImpl->ListProject();
    return result;
}

void DatahubClient::CreateProject(const std::string& project, const std::string& comment)
{
    mDatahubClientImpl->CreateProject(project, comment);
}

void DatahubClient::DeleteProject(const std::string& project)
{
    mDatahubClientImpl->DeleteProject(project);
}

GetProjectResult DatahubClient::GetProject(const std::string& project)
{
    return mDatahubClientImpl->GetProject(project);
}

void DatahubClient::UpdateProject(const std::string& project, const std::string& comment)
{
    mDatahubClientImpl->UpdateProject(project, comment);
}

ListTopicResult DatahubClient::ListTopic(const std::string& project)
{
    return mDatahubClientImpl->ListTopic(project);
}

void DatahubClient::CreateTopic(
    const std::string& project,
    const std::string& topic,
    int shardCount,
    int lifeCycle,
    const RecordType& type,
    const RecordSchema& schema,
    const std::string& comment)
{
    mDatahubClientImpl->CreateTopic(project, topic, shardCount, lifeCycle, type, schema, comment);
}

void DatahubClient::CreateTopic(
    const std::string& project,
    const std::string& topic,
    int shardCount,
    int lifeCycle,
    const RecordType& type,
    const std::string& comment)
{
    mDatahubClientImpl->CreateTopic(project, topic, shardCount, lifeCycle, type, comment);
}

void DatahubClient::DeleteTopic(const std::string& project, const std::string& topic)
{
    mDatahubClientImpl->DeleteTopic(project, topic);
}

void DatahubClient::UpdateTopic(
        const std::string& project,
        const std::string& topic,
        int lifeCycle,
        const std::string& comment)
{
    mDatahubClientImpl->UpdateTopic(project, topic, lifeCycle, comment);
}

GetTopicResult DatahubClient::GetTopic(const std::string& project, const std::string& topic)
{
    return mDatahubClientImpl->GetTopic(project, topic);
}

void DatahubClient::AppendField(
        const std::string& project,
        const std::string& topic,
        const std::string& fieldName,
        const std::string& fieldType)
{
    mDatahubClientImpl->AppendField(project, topic, fieldName, fieldType);
}

ListShardResult DatahubClient::ListShard(
        const std::string& project, const std::string& topic)
{
    return mDatahubClientImpl->ListShard(project, topic);
}

void DatahubClient::ActivateShard(
        const std::string& project,
        const std::string& topic,
        const StringVec& shards)
{
    mDatahubClientImpl->ActivateShard(project, topic, shards);
}

SplitShardResult DatahubClient::SplitShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& splitKey)
{
    return mDatahubClientImpl->SplitShard(project, topic, shardId, splitKey);
}

MergeShardResult DatahubClient::MergeShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& adjacentShardId)
{
    return mDatahubClientImpl->MergeShard(project, topic, shardId, adjacentShardId);
}

void DatahubClient::WaitForShardReady(
        const std::string& project,
        const std::string& topic,
        uint64_t timeout)
{
    mDatahubClientImpl->WaitForShardReady(project, topic, timeout);
}

GetMeteringInfoResult DatahubClient::GetMeteringInfo(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId)
{
    return mDatahubClientImpl->GetMeteringInfo(project, topic, shardId);
}

GetTopicMeteringInfoResult DatahubClient::GetTopicMeteringInfo(
        const std::string& project,
        const std::string& topic,
        const std::string& day)
{
    return mDatahubClientImpl->GetTopicMeteringInfo(project, topic, day);
}

PutRecordResult DatahubClient::PutRecord(const std::string& project,
        const std::string& topic,
        const std::vector<RecordEntry>& records)
{
    return mDatahubClientImpl->PutRecord(project, topic, records);
}

PutRecordByShardResult DatahubClient::PutRecordByShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::vector<RecordEntry>& records)
{
    return mDatahubClientImpl->PutRecordByShard(project, topic, shardId, records);
}

GetCursorResult DatahubClient::GetCursor(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const CursorType& type)
{
    return mDatahubClientImpl->GetCursor(project, topic, shardId, type);
}

GetCursorResult DatahubClient::GetCursor(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        int64_t timestamp)
{
    return mDatahubClientImpl->GetCursor(project, topic, shardId, CURSOR_TYPE_TIMESTAMP, timestamp);
}

GetCursorResult DatahubClient::GetCursor(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const CursorType& type,
        int64_t value)
{
    return mDatahubClientImpl->GetCursor(project, topic, shardId, type, value);
}

GetRecordResult DatahubClient::GetRecord(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& cursor,
        int limit,
        const std::string& subId)
{
    return mDatahubClientImpl->GetRecord(project, topic, shardId, cursor, limit, subId);
}

ListConnectorResult DatahubClient::ListConnector(
        const std::string& project,
        const std::string& topic)
{
    return mDatahubClientImpl->ListConnector(project, topic);
}

CreateConnectorResult DatahubClient::CreateConnector(
        const std::string& project,
        const std::string& topic,
        const sdk::ConnectorType& type,
        int64_t sinkStartTime,
        const StringVec& columnFields,
        const sdk::SinkConfig& config)
{
    return mDatahubClientImpl->CreateConnector(project, topic, type, sinkStartTime, columnFields, config);
}

CreateConnectorResult DatahubClient::CreateConnector(
        const std::string& project,
        const std::string& topic,
        const sdk::ConnectorType& type,
        const StringVec& columnFields,
        const sdk::SinkConfig& config)
{
    return mDatahubClientImpl->CreateConnector(project, topic, type, columnFields, config);
}

GetConnectorResult DatahubClient::GetConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    return mDatahubClientImpl->GetConnector(project, topic, connectorId);
}

void DatahubClient::UpdateConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::SinkConfig& config)
{
    mDatahubClientImpl->UpdateConnector(project, topic, connectorId, config);
}

void DatahubClient::DeleteConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    mDatahubClientImpl->DeleteConnector(project, topic, connectorId);
}

GetConnectorDoneTimeResult DatahubClient::GetConnectorDoneTime(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    return mDatahubClientImpl->GetConnectorDoneTime(project, topic, connectorId);
}

void DatahubClient::ReloadConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    mDatahubClientImpl->ReloadConnector(project, topic, connectorId);
}

void DatahubClient::ReloadConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId)
{
    mDatahubClientImpl->ReloadConnector(project, topic, connectorId, shardId);
}

void DatahubClient::UpdateConnectorState(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::ConnectorState& state)
{
    mDatahubClientImpl->UpdateConnectorState(project, topic, connectorId, state);
}

void DatahubClient::UpdateConnectorOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::ConnectorOffset& connectorOffset)
{
    mDatahubClientImpl->UpdateConnectorOffset(project, topic, connectorId, connectorOffset);
}

void DatahubClient::UpdateConnectorOffsetByShard(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId,
        const sdk::ConnectorOffset& connectorOffset)
{
    mDatahubClientImpl->UpdateConnectorOffsetByShard(project, topic, connectorId, shardId, connectorOffset);
}

GetConnectorShardStatusResult DatahubClient::GetConnectorShardStatus(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    return mDatahubClientImpl->GetConnectorShardStatus(project, topic, connectorId);
}

GetConnectorShardStatusByShardResult DatahubClient::GetConnectorShardStatusByShard(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId)
{
    return mDatahubClientImpl->GetConnectorShardStatusByShard(project, topic, connectorId, shardId);
}

void DatahubClient::AppendConnectorField(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& fieldName)
{
    mDatahubClientImpl->AppendConnectorField(project, topic, connectorId, fieldName);
}

ListSubscriptionResult DatahubClient::ListSubscription(
        const std::string& project,
        const std::string& topic,
        int pageIndex,
        int pageSize,
        const std::string& search)
{
    return mDatahubClientImpl->ListSubscription(project, topic, pageIndex, pageSize, search);
}

ListSubscriptionResult DatahubClient::ListSubscription(
        const std::string& project,
        const std::string& topic,
        int pageIndex,
        int pageSize)
{
    return mDatahubClientImpl->ListSubscription(project, topic, pageIndex, pageSize);
}

CreateSubscriptionResult DatahubClient::CreateSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& comment)
{
    return mDatahubClientImpl->CreateSubscription(project, topic, comment);
}

void DatahubClient::DeleteSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& subId)
{
    mDatahubClientImpl->DeleteSubscription(project, topic, subId);
}

GetSubscriptionResult DatahubClient::GetSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& subId)
{
    return mDatahubClientImpl->GetSubscription(project, topic, subId);
}

void DatahubClient::UpdateSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::string& comment)
{
    mDatahubClientImpl->UpdateSubscription(project, topic, subId, comment);
}

void DatahubClient::UpdateSubscriptionState(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const SubscriptionState& state)
{
    mDatahubClientImpl->UpdateSubscriptionState(project, topic, subId, state);
}

OpenSubscriptionOffsetSessionResult DatahubClient::InitSubscriptionOffsetSession(
        const std::string &project,
        const std::string &topic,
        const std::string &subId,
        const StringVec& shardIds)
{
    return mDatahubClientImpl->InitSubscriptionOffsetSession(project, topic, subId, shardIds);
}

GetSubscriptionOffsetResult DatahubClient::GetSubscriptionOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const StringVec& shardIds)
{
    return mDatahubClientImpl->GetSubscriptionOffset(project, topic, subId, shardIds);
}

void DatahubClient::UpdateSubscriptionOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::map<std::string, SubscriptionOffset>& offsets)
{
    mDatahubClientImpl->UpdateSubscriptionOffset(project, topic, subId, offsets);
}

void DatahubClient::ResetSubscriptionOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::map<std::string, SubscriptionOffset>& offsets)
{
    mDatahubClientImpl->ResetSubscriptionOffset(project, topic, subId, offsets);
}

HeartbeatResult DatahubClient::Heartbeat(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId,
        const StringVec& holdShardList,
        const StringVec& readEndShardList)
{
    return mDatahubClientImpl->Heartbeat(project, topic, consumerGroup, consumerId, versionId, holdShardList, readEndShardList);
}

JoinGroupResult DatahubClient::JoinGroup(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        int64_t sessionTimeout)
{
    return mDatahubClientImpl->JoinGroup(project, topic, consumerGroup, sessionTimeout);
}

void DatahubClient::SyncGroup(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId,
        const StringVec& releaseShardList,
        const StringVec& readEndShardList)
{
    mDatahubClientImpl->SyncGroup(project, topic, consumerGroup, consumerId, versionId, releaseShardList, readEndShardList);
}

void DatahubClient::LeaveGroup(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId)
{
    mDatahubClientImpl->LeaveGroup(project, topic, consumerGroup, consumerId, versionId);
}

RegisterTopicSchemaResult DatahubClient::RegisterTopicSchema(
    const std::string& project,
    const std::string& topic,
    const RecordSchema& schema)
{
    return mDatahubClientImpl->RegisterTopicSchema(project, topic, schema);
}

GetTopicSchemaResult DatahubClient::GetTopicSchema(
    const std::string& project,
    const std::string& topic,
    const RecordSchema& schema)
{
    return mDatahubClientImpl->GetTopicSchema(project, topic, -1, schema);
}

GetTopicSchemaResult DatahubClient::GetTopicSchema(
    const std::string& project,
    const std::string& topic,
    int64_t versionId)
{
    return mDatahubClientImpl->GetTopicSchema(project, topic, versionId, RecordSchema());
}

ListTopicSchemaResult DatahubClient::ListTopicSchema(
    const std::string& project,
    const std::string& topic)
{
    return mDatahubClientImpl->ListTopicSchema(project, topic, -1, -1);
}

ListTopicSchemaResult DatahubClient::ListTopicSchema(
    const std::string& project,
    const std::string& topic,
    int pageNumber,
    int pageSize)
{
    return mDatahubClientImpl->ListTopicSchema(project, topic, pageNumber, pageSize);
}

DeleteTopicSchemaResult DatahubClient::DeleteTopicSchema(
    const std::string& project,
    const std::string& topic,
    int64_t versionId)
{
    return mDatahubClientImpl->DeleteTopicSchema(project, topic, versionId);
}

DatahubMetric DatahubClient::GetReadMetric() const
{
    return mDatahubClientImpl->GetReadMetric();
}

DatahubMetric DatahubClient::GetWriteMetric() const
{
    return mDatahubClientImpl->GetWriteMetric();
}

} // namespace datahub
} // namespace aliyun
