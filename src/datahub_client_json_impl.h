#ifndef DATAHUB_SDK_DATAHUB_CLIENT_JSON_IMPL_H
#define DATAHUB_SDK_DATAHUB_CLIENT_JSON_IMPL_H

#include <string>
#include <memory>
#include "auth_signer.h"
#include "http/http_client.h"
#include "http/http_request.h"
#include "datahub/datahub_config.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_result.h"
#include "datahub_client_impl.h"

namespace aliyun
{
namespace datahub
{

class DatahubClientJsonImpl : public DatahubClientImpl
{
public:
    explicit DatahubClientJsonImpl(const Configuration& conf);

    virtual ~DatahubClientJsonImpl();

    virtual ListProjectResult ListProject();

    virtual void CreateProject(const std::string& project, const std::string& comment);

    virtual void DeleteProject(const std::string& project);

    virtual GetProjectResult GetProject(const std::string& project);

    virtual void UpdateProject(const std::string& project, const std::string& comment);

    virtual ListTopicResult ListTopic(const std::string& project);

    virtual void CreateTopic(const std::string& project,
            const std::string& topic,
            int shardCount,
            int lifeCycle,
            const RecordType& type,
            const RecordSchema& schema,
            const std::string& comment);

    virtual void CreateTopic(const std::string& project,
            const std::string& topic,
            int shardCount,
            int lifeCycle,
            const RecordType& type,
            const std::string& comment);

    virtual void DeleteTopic(const std::string& project, const std::string& topic);

    virtual void UpdateTopic(
            const std::string& project,
            const std::string& topic,
            int lifeCycle,
            const std::string& comment);

    virtual GetTopicResult GetTopic(const std::string& project, const std::string& topic);

    virtual void AppendField(
            const std::string& project,
            const std::string& topic,
            const std::string& fieldName,
            const std::string& fieldType);

    virtual ListShardResult ListShard(
            const std::string& project, const std::string& topic);

    virtual void ActivateShard(
            const std::string& project,
            const std::string& topic,
            const StringVec& shards);

    virtual SplitShardResult SplitShard(const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& splitKey);

    virtual MergeShardResult MergeShard(const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& adjacentShardId);

    virtual void WaitForShardReady(
            const std::string& project,
            const std::string& topic,
            uint64_t timeout = 120);

    virtual GetMeteringInfoResult GetMeteringInfo(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId);

    virtual GetTopicMeteringInfoResult GetTopicMeteringInfo(
            const std::string& project,
            const std::string& topic,
            const std::string& day);

    virtual PutRecordResult PutRecord(
            const std::string& project,
            const std::string& topic,
            const std::vector<RecordEntry>& records);

    virtual PutRecordByShardResult PutRecordByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::vector<RecordEntry>& records);

    virtual GetCursorResult GetCursor(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const CursorType& type);

    virtual GetCursorResult GetCursor(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const CursorType& type,
            int64_t value);

    virtual GetRecordResult GetRecord(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& cursor,
            int limit = 10,
            const std::string& subId = "");

    virtual ListConnectorResult ListConnector(
            const std::string& project,
            const std::string& topic);

    virtual CreateConnectorResult CreateConnector(
            const std::string& project,
            const std::string& topic,
            const sdk::ConnectorType& type,
            int64_t sinkStartTime,
            const StringVec& columnFields,
            const sdk::SinkConfig& config);

    virtual CreateConnectorResult CreateConnector(
            const std::string& project,
            const std::string& topic,
            const sdk::ConnectorType& type,
            const StringVec& columnFields,
            const sdk::SinkConfig& config);

    virtual GetConnectorResult GetConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual void UpdateConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::SinkConfig& config);

    virtual void DeleteConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual GetConnectorDoneTimeResult GetConnectorDoneTime(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual void ReloadConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual void ReloadConnector(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId);

    virtual void UpdateConnectorState(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::ConnectorState& state);

    virtual void UpdateConnectorOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::ConnectorOffset& connectorOffset);

    virtual void UpdateConnectorOffsetByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId,
            const sdk::ConnectorOffset& connectorOffset);

    virtual GetConnectorShardStatusResult GetConnectorShardStatus(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual GetConnectorShardStatusByShardResult GetConnectorShardStatusByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId);

    virtual void AppendConnectorField(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& fieldName);

    virtual ListSubscriptionResult ListSubscription(
            const std::string& project,
            const std::string& topic,
            int pageIndex,
            int pageSize,
            const std::string& search);

    virtual ListSubscriptionResult ListSubscription(
            const std::string& project,
            const std::string& topic,
            int pageIndex,
            int pageSize);

    virtual CreateSubscriptionResult CreateSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& comment);

    virtual void DeleteSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& subId);

    virtual GetSubscriptionResult GetSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& subId);

    virtual void UpdateSubscription(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::string& comment);

    virtual void UpdateSubscriptionState(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const SubscriptionState& state);

    virtual OpenSubscriptionOffsetSessionResult InitSubscriptionOffsetSession(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const StringVec& shardIds);

    virtual GetSubscriptionOffsetResult GetSubscriptionOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const StringVec& shardIds);

    virtual void UpdateSubscriptionOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::map<std::string, SubscriptionOffset>& offsets);

    virtual void ResetSubscriptionOffset(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::map<std::string, SubscriptionOffset>& offsets);

    virtual HeartbeatResult Heartbeat(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId,
            const StringVec& holdShardList,
            const StringVec& readEndShardList);

    virtual JoinGroupResult JoinGroup(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            int64_t sessionTimeout);

    virtual void SyncGroup(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId,
            const StringVec& releaseShardList,
            const StringVec& readEndShardList);

    virtual void LeaveGroup(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId);

    virtual RegisterTopicSchemaResult RegisterTopicSchema(
        const std::string& project,
        const std::string& topic,
        const RecordSchema& schema);

    virtual GetTopicSchemaResult GetTopicSchema(
        const std::string& project,
        const std::string& topic,
        int64_t versionId,
        const RecordSchema& schema);

    virtual ListTopicSchemaResult ListTopicSchema(
        const std::string& project,
        const std::string& topic,
        int pageNumber,
        int pageSize);

    virtual DeleteTopicSchemaResult DeleteTopicSchema(
        const std::string& project,
        const std::string& topic,
        int64_t versionId);

protected:
    bool IsShardLoadCompleted(const std::string& project, const std::string& topic);

    void BuildHttpRequest(const DatahubRequest& request,
            const std::shared_ptr<http::HttpRequest>& httpRequest) const;

    void AddHeadersToRequest(const std::shared_ptr<http::HttpRequest>& httpRequest,
            const StringMap& headerValues) const;

    void AddContentBodyToRequest(const std::shared_ptr<http::HttpRequest>& httpRequest,
            const std::string& body, uint32_t compressMode) const;

    void AddCommonHeaders(http::HttpRequest& httpRequest) const;

    std::shared_ptr<http::HttpResponse> AttemptOneRequest(
            const DatahubRequest& request, const http::HttpMethod& method);

    void ParseResponse(const std::shared_ptr<http::HttpResponse>& response, DatahubResult& result);

protected:
    bool mUseV4Sign;
    int mCompressLevel;
    compress::CompressMethod mCompress;
    std::shared_ptr<http::HttpClient> mHttpClient;
    std::string mEndpoint;
    std::string mContentType;
    std::string mUserAgent;
    std::string mVersion;
    AuthSigner  mSigner;
};

} // namespace datahub
} // namespace aliyun
#endif
