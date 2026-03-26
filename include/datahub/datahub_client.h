#ifndef DATAHUB_SDK_DATAHUB_CLIENT_H
#define DATAHUB_SDK_DATAHUB_CLIENT_H

#include "datahub/datahub_record.h"
#include "datahub/datahub_shard.h"
#include "datahub/datahub_config.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_result.h"

namespace aliyun
{
namespace datahub
{

class DatahubClientImpl;
class DatahubClient
{
public:
    explicit DatahubClient(const Configuration& conf);
    virtual ~DatahubClient();

    /**
     * List all projects the user owns.
     *
     * @return Result of the ListProject operation returned by the service.
     */
    virtual ListProjectResult ListProject();

    /**
     * Create a Datahub project.
     * The concept of project is used to serve multiple tenants.
     *
     * @param project
     *         The name of the project.
     * @param comment
     *         The description of the project.
     */
    virtual void CreateProject(const std::string& project, const std::string& comment);

    /**
     * Delete a project and all its topics and data.
     *
     * @param project
     *        The name of the project.
     */
    virtual void DeleteProject(const std::string& project);

    /**
     * Get the information of the specified project.
     *
     * @param project
     *        The name of the project.
     * @return Result of the GetProject operation returned by the service.
     */
    virtual GetProjectResult GetProject(const std::string& project);

    /**
     * Update a Datahub project.
     *
     * @param project
     *         The name of the project.
     * @param comment
     *         The description of the project.
     */
    virtual void UpdateProject(const std::string& project, const std::string& comment);

    /**
    * List all topics in the project.
    *
    * @param project
    *        The name of the project.
     * @return Result of the ListTopic operation returned by the service.
    */
    virtual ListTopicResult ListTopic(const std::string& project);

    /**
     *
     * Create a Datahub topic.
     *
     *
     * A topic stores data records from different data
     * sources. Scale-out within a Datahub topic is explicitly supported by
     * means of shards.
     *
     *
     * <code>CreateTopic</code> is an asynchronous operation. When receiving a
     * <code>CreateTopic</code> request, service immediately returns. After the
     * shards created, service sets the shards status to <code>ACTIVE</code>.
     * You should perform write operations only on an <code>ACTIVE</code> shards.
     *
     *
     * You can use <code>ListShard</code> to check the shard status, which
     * is returned in <code>StatusEntry</code>.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param shardCount
     *        The initial shard count of the topic.
     * @param lifeCycle
     *        The data records expired time after which data can not be accessible.
     *        Unit of time represents in day.
     * @param type
     *        The type of the topic, now support TUPLE only.
     * @param schema
     *        The schema of the topic.
     * @param comment
     *        The desc for the topic.
     */
    virtual void CreateTopic(const std::string& project,
            const std::string& topic,
            int shardCount,
            int lifeCycle,
            const RecordType& type,
            const RecordSchema& schema,
            const std::string& comment);

    /**
     *
     * Create a Datahub topic.
     *
     *
     * A topic stores data records from different data
     * sources. Scale-out within a Datahub topic is explicitly supported by
     * means of shards.
     *
     *
     * <code>CreateTopic</code> is an asynchronous operation. When receiving a
     * <code>CreateTopic</code> request, service immediately returns. After the
     * shards created, service sets the shards status to <code>ACTIVE</code>.
     * You should perform write operations only on an <code>ACTIVE</code> shards.
     *
     *
     * You can use <code>ListShard</code> to check the shard status, which
     * is returned in <code>StatusEntry</code>.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param shardCount
     *        The initial shard count of the topic.
     * @param lifeCycle
     *        The data records expired time after which data can not be accessible.
     *        Unit of time represents in day.
     * @param type
     *        The type of the topic, now support TUPLE only.
     * @param comment
     *        The desc for the topic.
     */
    virtual void CreateTopic(const std::string& project,
            const std::string& topic,
            int shardCount,
            int lifeCycle,
            const RecordType& type,
            const std::string& comment);

    /**
     * Delete a topic and all its shards and data.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     */
    virtual void DeleteTopic(const std::string& project, const std::string& topic);

    /**
     * Update a Datahub topic.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param lifeCycle
     *        The data records expired time after which data can not be accessible.
     *        Unit of time represents in day.
     * @param comment
     *        The desc for the topic.
     */
    virtual void UpdateTopic(
            const std::string& project,
            const std::string& topic,
            int lifeCycle,
            const std::string& comment);

    /**
     * Get the specified topic information.
     *
     *
     * The information about the topic includes its shard count, its lifecycle,
     * its record type, its record schema, its created time, its last modified
     * time and its description.
     *
     * @param project
     *        The name of project.
     * @param topic
     *        The name of topic.
     * @return Result of the GetTopic operation returned by the service.
     */
    virtual GetTopicResult GetTopic(const std::string& project, const std::string& topic);

    /**
     * Modify TopicSchema, add a field at the end.
     *
     * @param project
     *        The name of project.
     * @param topic
     *        The name of topic.
     * @param fieldName
     *        The name of field.
     * @param fieldType
     *        The type of field.
     */
    virtual void AppendField(
            const std::string& project,
            const std::string& topic,
            const std::string& fieldName,
            const std::string& fieldType);

    /**
     * List shard information of a topic.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @return Result of the ListShard operation returned by the service.
     */
    virtual ListShardResult ListShard(
            const std::string& project, const std::string& topic);

    /**
     * Activate shards.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param shards
     *        The array of shard ids.
     */
    virtual void ActivateShard(const std::string& project,
            const std::string& topic,
            const StringVec& shards);

    /**
     * Split shards.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param shardId
     *        The shard to split.
     * @param splitKey
     *        The split key.
     * @return Result of the SplitShard operation returned by the service.
     */
    virtual SplitShardResult SplitShard(const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& splitKey);

    /**
     * Merge the specified shard and its adjacent shard. Only adjacent shards can be merged.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param shardId
     *        The shard to merge.
     * @param adjacentShardId
     *        The adjacentShard to be merged.
     * @return Result of the MergeShard operation returned by the service.
     */
    virtual MergeShardResult MergeShard(const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& adjacentShardId);

    /**
     * Wait for all shard ready.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param timeout
     *         The wait time for shard active.
     */
    virtual void WaitForShardReady(
            const std::string& project,
            const std::string& topic,
            uint64_t timeout = 120);

    /**
     * Get metering info from a shard.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param shardId
     *         The shard ID of shard.
     * @return Result of the GetMeteringInfo operation returned by the service.
     */
    virtual GetMeteringInfoResult GetMeteringInfo(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId);

    /**
     * Get metering info from a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param day
     *         The day of metering, example: "20190101".
     * @return Result of the GetTopicMeteringInfoResult operation returned by the service.
     */
    virtual GetTopicMeteringInfoResult GetTopicMeteringInfo(
            const std::string& project,
            const std::string& topic,
            const std::string& day);

    /**
     *
     * Write data records into a Datahub topic.
     *
     *
     * The response includes unsuccessfully processed records. Datahub attempts to
     * process all records in each <code>PutRecords</code> request. A single record
     * failure does not stop the processing of subsequent records.
     *
     *
     * An unsuccessfully-processed record includes <code>ErrorCode</code> and
     * <code>ErrorMessage</code> values.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param records
     *        The array of records sent to service.
     * @return Result of the PutRecords operation returned by the service.
     */
    virtual PutRecordResult PutRecord(
            const std::string& project,
            const std::string& topic,
            const std::vector<RecordEntry>& records);

    /**
     *
     * Write data records into a shard of Datahub topic.
     *
     *
     * The response is empty if all records is successfully processed. Or all failure
     * with an exception.
     *
     * An unsuccessfully-processed record includes <code>ErrorCode</code> and
     * <code>ErrorMessage</code> values.
     *
     * @param project
     *        The name of the project.
     * @param topic
     *        The name of the topic.
     * @param shardId
     *        The shard id to put records.
     * @param records
     *        The array of records sent to service.
     */
    virtual PutRecordByShardResult PutRecordByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::vector<RecordEntry>& records);

    /**
     * Get a shard cursor.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param shardId
     *         The shard ID of shard.
     * @param type
     *         The cursor type. OLDEST/LATEST
     * @return Result of the GetCursor operation returned by the service.
     */
    virtual GetCursorResult GetCursor(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const CursorType& type);

    /**
     * Get a shard cursor.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param shardId
     *         The shard ID of shard.
     * @param timestamp
     *         start reading records which received at the most recent time after 'timestamp',
     *         represents as unix epoch in millisecond.
     * @return Result of the GetCursor operation returned by the service.
     */
    virtual GetCursorResult GetCursor(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            int64_t timestamp);

    /**
     * Get a shard cursor.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param shardId
     *         The shard ID of shard.
     * @param type
     *         The cursor type. TIMESTAMP/SEQUENCE
     * @param value
     *         For TIMSTAMP,start reading records which received at the most recent time after 'timestamp',
     *         represents as unix epoch in millisecond;
     *         For SEQUENCE, start reading reacords with the given sequence.
     * @return Result of the GetCursor operation returned by the service.
     */
    virtual GetCursorResult GetCursor(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const CursorType& type,
            int64_t value);

    /**
     * Get data records from a shard.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param shardId
     *         The shard ID of shard.
     * @param cursor
     *         The value of cursor.
     * @param limit
     *         The maximum number of records returned by service.
     * @return Result of the GetRecords operation returned by the service.
     */
    virtual GetRecordResult GetRecord(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& cursor,
            int limit = 10,
            const std::string& subId = "");

    /**
     * List name of connectors.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @return Result of the ListConnector operation returned by the service.
     */
    virtual ListConnectorResult ListConnector(
            const std::string& project,
            const std::string& topic);

    /**
     * Create datahub data connectors.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param type
     *         The type of connector which you want create.
     * @param sinkStartTime
     *         Start time to sink from datahub. Unit: Ms
     * @param columnFields
     *         Which fields you want synchronize.
     * @param config
     *         Detail config of specified connector type.
     * @return Result of the CreateConnector operation returned by the service.
     */
    virtual CreateConnectorResult CreateConnector(
            const std::string& project,
            const std::string& topic,
            const sdk::ConnectorType& type,
            int64_t sinkStartTime,
            const StringVec& columnFields,
            const sdk::SinkConfig& config);

    /**
     * Create datahub data connectors.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param type
     *         The type of connector which you want create.
     * @param columnFields
     *         Which fields you want synchronize.
     * @param config
     *         Detail config of specified connector type.
     * @return Result of the CreateConnector operation returned by the service.
     */
    virtual CreateConnectorResult CreateConnector(
            const std::string& project,
            const std::string& topic,
            const sdk::ConnectorType& type,
            const StringVec& columnFields,
            const sdk::SinkConfig& config);

    /**
     * Get information of the specified data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @return Result of the GetConnector operation returned by the service.
     */
    virtual GetConnectorResult GetConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    /**
     * Update connector config of the specified data connector
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param config
     *         Detail config of specified connector type.
     */
    virtual void UpdateConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::SinkConfig& config);

    /**
     * Delete a data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     */
    virtual void DeleteConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    /**
     * Get the done time of a data connector.
     * This method mainly used to get MaxCompute synchronize point.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @return Result of the GetConnectorDoneTime operation returned by the service.
     */
    virtual GetConnectorDoneTimeResult GetConnectorDoneTime(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    /**
     * Reload a data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     */
    virtual void ReloadConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    /**
     * Reload the specified shard of the data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param shardId
     *         The id of the shard.
     */
    virtual void ReloadConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId);

    /**
     * Update the state of the data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param state
     *         The state of the connector. Support: CONNECTOR_STOPPED, CONNECTOR_RUNNING.
     */
    virtual void UpdateConnectorState(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::ConnectorState& state);

    /**
     * Update connector sink offset. The operation must be operated after connector stopped.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param timestamp
     *         The timestamp of connector offset.
     * @param sequence
     *         The timestamp of connector offset.
     */
    virtual void UpdateConnectorOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::ConnectorOffset& connectorOffset);

    /**
     * Update connector sink offset. The operation must be operated after connector stopped.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param shardId
     *         The id of the shard.
     * @param connectorOffset
     *         The connector offset.
     */
    virtual void UpdateConnectorOffsetByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId,
            const sdk::ConnectorOffset& connectorOffset);

    /**
     * Get the detail information of the shard task which belongs to the specified data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @return Result of the GetConnectorShardStatus operation returned by the service.
     */
    virtual GetConnectorShardStatusResult GetConnectorShardStatus(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    /**
     * Get the detail information of the shard task which belongs to the specified data connector.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param shardId
     *         The id of the shard.
     * @return Result of the GetConnectorShardStatusByShard operation returned by the service.
     */
    virtual GetConnectorShardStatusByShardResult GetConnectorShardStatusByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId);

    /**
     * Append data connector field.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param connectorId
     *         The id of the connector.
     * @param fieldName
     *         The name of the field.
     */
    virtual void AppendConnectorField(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& fieldName);

    /**
     * list subscription for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param pageIndex
     *         Page index, starting from 1.
     * @param pageSize
     *         Page size.
     * @param search
     *         SubId search keyword.
     * @return Result of the ListSubscription operation returned by the service.
     */
    virtual ListSubscriptionResult ListSubscription(
            const std::string& project,
            const std::string& topic,
            int pageIndex,
            int pageSize,
            const std::string& search);

    /**
     * list subscription for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param pageIndex
     *         Page index, starting from 1.
     * @param pageSize
     *         Page size.
     * @return Result of the ListSubscription operation returned by the service.
     */
    virtual ListSubscriptionResult ListSubscription(
            const std::string& project,
            const std::string& topic,
            int pageIndex,
            int pageSize);

    /**
     * Create a subscription for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param comment
     *         The comment of subscription.
     * @return Result of the CreateSubscription operation returned by the service.
     */
    virtual CreateSubscriptionResult CreateSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& comment);

    /**
     * Delete a subscription for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     */
    virtual void DeleteSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& subId);

    /**
     * Get a subscription for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     * @return Result of the GetSubscription operation returned by the service.
     */
    virtual GetSubscriptionResult GetSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& subId);

    /**
     * Update a subscription for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     */
    virtual void UpdateSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::string& comment);

    /**
     * Update a subscription state for a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     * @param state
     *         The state of subscription.
     */
    virtual void UpdateSubscriptionState(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const SubscriptionState& state);

    /**
     * Open a subscription offset session and get subscription offsets.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     * @param shardIds
     *         The shards of a subscription.
     * @return Result of the InitSubscriptionOffsetSession operation returned by the service.
     */
    virtual OpenSubscriptionOffsetSessionResult InitSubscriptionOffsetSession(
            const std::string &project,
            const std::string &topic,
            const std::string &subId,
            const StringVec& shardIds);

    /**
     * Get subscription offset.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     * @param shardIds
     *         The shards of a subscription.
     * @return Result of the GetSubscriptionOffset operation returned by the service.
     */
    virtual GetSubscriptionOffsetResult GetSubscriptionOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const StringVec& shardIds);

    /**
     *  Update subscription offset.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     * @param offsets
     *         The subscription offset info.
     */
    virtual void UpdateSubscriptionOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::map<std::string, SubscriptionOffset>& offsets);

    /**
     *  Reset subscription offset.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param subId
     *         The ID of subscription.
     * @param offsets
     *         The subscription offset info.
     */
    virtual void ResetSubscriptionOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::map<std::string, SubscriptionOffset>& offsets);

    /**
     *  Heartbeat request to let server know consumer status.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param consumerGroup
     *         The consumerGroup use subId.
     * @param consumerId
     *         The consumerId.
     * @param versionId
     *         The offset version id.
     * @param holdShardList
     *         The shard list held by consumer.
     * @param readEndShardList
     *         The shard list finished.
     * @return Result of the Heartbeat operation returned by the service.
     */
    virtual HeartbeatResult Heartbeat(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId,
            const StringVec& holdShardList,
            const StringVec& readEndShardList);

    /**
     *  Join a consumer group.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param consumerGroup
     *         The consumerGroup use subId.
     * @param sessionTimeout
     *         The session timeout.
     * @return Result of the JoinGroup operation returned by the service.
     */
    virtual JoinGroupResult JoinGroup(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            int64_t sessionTimeout);

    /**
     *  Sync consumer group info.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param consumerGroup
     *         The consumerGroup use subId.
     * @param consumerId
     *         The consumerId.
     * @param versionId
     *         The offset version id.
     * @param releaseShardList
     *         The shard list to release.
     * @param readEndShardList
     *         The shard list finished.
     */
    virtual void SyncGroup(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId,
            const StringVec& releaseShardList,
            const StringVec& readEndShardList);

    /**
     *  Leave consumer group info.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param consumerGroup
     *         The consumerGroup use subId.
     * @param consumerId
     *         The consumerId.
     * @param versionId
     *         The offset version id.
     */
    virtual void LeaveGroup(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId);

    /**
     * Register schema to a topic.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param schem
     *         The topic schema to register.
     */
    virtual RegisterTopicSchemaResult RegisterTopicSchema(
        const std::string& project,
        const std::string& topic,
        const RecordSchema& schema);

    /**
     * Get schema by schema string.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param schema
     *         The schema used to get versionId.
     */
    virtual GetTopicSchemaResult GetTopicSchema(
        const std::string& project,
        const std::string& topic,
        const RecordSchema& schema);

    /**
     * Get schema by versionId.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param versionId
     *         The versionId of schema.
     */
    virtual GetTopicSchemaResult GetTopicSchema(
        const std::string& project,
        const std::string& topic,
        int64_t versionId);

    /**
     * List all topic schema.
     *
     * @param project
    *         The name of the project.
    * @param topic
    *         The name of topic.
     */
    virtual ListTopicSchemaResult ListTopicSchema(
        const std::string& project,
        const std::string& topic);

    /**
     * List topic schema by page.
     *
     * @param project
     *         The name of the project.
     * @param topic
     *         The name of topic.
     * @param pageNumber
     *         The number of page.
     * @param pageSize
     *         The size of page.
     */
    virtual ListTopicSchemaResult ListTopicSchema(
        const std::string& project,
        const std::string& topic,
        int pageNumber,
        int pageSize);

   /**
    * Delete topic schema by versionId
    *
    * @param project
    *         The name of the project.
    * @param topic
    *         The name of topic.
    * @param versionId
    *         The versionId of schema.
    */
    virtual DeleteTopicSchemaResult DeleteTopicSchema(
        const std::string& project,
        const std::string& topic,
        int64_t versionId);

    DatahubMetric GetReadMetric(void) const;
    DatahubMetric GetWriteMetric(void) const;

private:
    DatahubClientImpl* mDatahubClientImpl;
};

typedef std::shared_ptr<DatahubClient> DatahubClientPtr;

} // namespace datahub
} // namespace aliyun
#endif
