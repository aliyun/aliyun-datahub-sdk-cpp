#ifndef DATAHUB_SDK_DATAHUB_REQUEST_H
#define DATAHUB_SDK_DATAHUB_REQUEST_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include "datahub/datahub_record.h"
#include "datahub/datahub_compressor.h"
#include "datahub/datahub_subscription.h"
#include "datahub/datahub_subscription_offset.h"
#include "datahub/datahub_connector.h"
#include "datahub/datahub_typedef.h"

namespace aliyun
{
namespace datahub
{

enum CursorType
{
    CURSOR_TYPE_OLDEST,
    CURSOR_TYPE_LATEST,
    CURSOR_TYPE_TIMESTAMP,
    CURSOR_TYPE_SEQUENCE
};

#define ALLOW_SEND_COMPRESS 0x01
#define ALLOW_RECV_COMPRESS 0x02

std::string GetNameForCursorType(const CursorType& cursorType);

class DatahubRequest
{
public:
    DatahubRequest() {}
    virtual ~DatahubRequest() {}

    virtual std::string GetPath() const;

    virtual StringMap GetHeaders() const;

    virtual std::string GetBody() const;

    virtual StringMap GetRequestParameters() const;

    virtual StringMap GetRequestSpecificHeaders() const;

    virtual std::string BuildPath() const = 0;

    virtual std::string SerializePayload() const = 0;

    virtual uint32_t GetCompressMode() const { return 0; }
};

class ListProjectRequest : public DatahubRequest
{
public:
    ListProjectRequest();

    virtual ~ListProjectRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;
};

class CreateProjectRequest : public DatahubRequest
{
public:
    CreateProjectRequest(const std::string& project, const std::string& comment);

    virtual ~CreateProjectRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mComment;
};

class DeleteProjectRequest : public DatahubRequest
{
public:
    explicit DeleteProjectRequest(const std::string& project);

    virtual ~DeleteProjectRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
};

class GetProjectRequest : public DatahubRequest
{
public:
    explicit GetProjectRequest(const std::string& project);

    virtual ~GetProjectRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
};

class UpdateProjectRequest : public DatahubRequest
{
public:
    UpdateProjectRequest(const std::string& project, const std::string& comment);

    virtual ~UpdateProjectRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mComment;
};

class ListTopicRequest : public DatahubRequest
{
public:
    explicit ListTopicRequest(const std::string& project);

    virtual ~ListTopicRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
};

class CreateTopicRequest : public DatahubRequest
{
public:
    CreateTopicRequest(const std::string& project,
            const std::string& topic,
            int shardCount,
            int lifeCycle,
            const RecordType& type,
            const RecordSchema& schema,
            const std::string& comment);

    CreateTopicRequest(const std::string& project,
            const std::string& topic,
            int shardCount,
            int lifeCycle,
            const RecordType& type,
            const std::string& comment);

    virtual ~CreateTopicRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    int mShardCount;
    int mLifeCycle;
    RecordType mRecordType;
    RecordSchema mRecordSchema;
    std::string mComment;
};

class DeleteTopicRequest : public DatahubRequest
{
public:
    DeleteTopicRequest(const std::string& project, const std::string& topic);

    virtual ~DeleteTopicRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
};

class UpdateTopicRequest : public DatahubRequest
{
public:
    UpdateTopicRequest(const std::string& project,
                       const std::string& topic,
                       int lifeCycle,
                       const std::string& comment);

    virtual ~UpdateTopicRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    int mLifeCycle;
    std::string mComment;
};

class GetTopicRequest : public DatahubRequest
{
public:
    GetTopicRequest(const std::string& project, const std::string& topic);

    virtual ~GetTopicRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
};

class AppendFieldRequest : public DatahubRequest
{
public:
    AppendFieldRequest(const std::string& project,
                       const std::string& topic,
                       const std::string& fieldName,
                       const std::string& fieldType);

    virtual ~AppendFieldRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    std::string mFieldName;
    std::string mFieldType;
};

class ListShardRequest : public DatahubRequest
{
public:
    ListShardRequest(const std::string& project, const std::string& topic);

    virtual ~ListShardRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
};

class ActivateShardRequest : public DatahubRequest
{
public:
    ActivateShardRequest(const std::string& project, const std::string& topic,
            const StringVec& shards);

    virtual ~ActivateShardRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    StringVec mShards;
};

class SplitShardRequest : public DatahubRequest
{
public:
    SplitShardRequest(const std::string& project, const std::string& topic,
                      const std::string& shardId, const std::string& splitKey);

    virtual ~SplitShardRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mShardId;
    std::string mSplitKey;
};

class MergeShardRequest : public DatahubRequest
{
public:
    MergeShardRequest(const std::string& project, const std::string& topic,
            const std::string& shardId, const std::string& adjacentShardId);

    virtual ~MergeShardRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mShardId;
    std::string mAdjacentShardId;
};

class GetMeteringInfoRequest : public DatahubRequest
{
public:
    GetMeteringInfoRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId);

    virtual ~GetMeteringInfoRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mShardId;
    std::string mAction;
};

class GetTopicMeteringInfoRequest : public DatahubRequest
{
public:
    GetTopicMeteringInfoRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& day);

    virtual ~GetTopicMeteringInfoRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mDay;
    std::string mAction;
};

class PutRecordRequest : public DatahubRequest
{
public:
    PutRecordRequest(const std::string& project, const std::string& topic,
            const std::vector<RecordEntry>& records);

    virtual ~PutRecordRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

    virtual uint32_t GetCompressMode() const { return ALLOW_SEND_COMPRESS; }

protected:
    std::string mProject;
    std::string mTopic;
    const std::vector<RecordEntry>& mRecords;
};

class PutPBRecordRequest : public PutRecordRequest
{
public:
    PutPBRecordRequest(const std::string& project, const std::string& topic,
            const std::vector<RecordEntry>& records);

    virtual ~PutPBRecordRequest();

    virtual StringMap GetRequestSpecificHeaders() const;

    virtual std::string SerializePayload() const;

    virtual uint32_t GetCompressMode() const { return ALLOW_SEND_COMPRESS; }
};

class PutRecordByShardRequest : public DatahubRequest
{
public:
    PutRecordByShardRequest(const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::vector<RecordEntry>& records);

    virtual ~PutRecordByShardRequest();

    virtual StringMap GetRequestSpecificHeaders() const;

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

    virtual uint32_t GetCompressMode() const { return ALLOW_SEND_COMPRESS; }

private:
    std::string mProject;
    std::string mTopic;
    std::string mShardId;
    const std::vector<RecordEntry>& mRecords;
};

class GetCursorRequest : public DatahubRequest
{
public:
    GetCursorRequest(const std::string& project, const std::string& topic,
                     const std::string& shardId, const CursorType& type);

    GetCursorRequest(const std::string& project, const std::string& topic,
                     const std::string& shardId, long value, const CursorType& type = CURSOR_TYPE_TIMESTAMP);

    virtual ~GetCursorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mShardId;
    CursorType mType;
    long mValue;
};

class GetRecordRequest : public DatahubRequest
{
public:
    GetRecordRequest(const std::string& project, const std::string& topic,
                     const std::string& shardId, const std::string& cursor, int limit);

    virtual ~GetRecordRequest();

    virtual std::string BuildPath() const;

    virtual StringMap GetRequestSpecificHeaders() const;

    virtual std::string SerializePayload() const;

    virtual uint32_t GetCompressMode() const { return ALLOW_RECV_COMPRESS; }

    void SetFilter(const std::string& filter) { mFilter = filter; }
    void SetCursor(const std::string& cursor) { mCursor = cursor; }
    void SetSubId(const std::string& subId) { mSubId = subId; }

protected:
    std::string mProject;
    std::string mTopic;
    std::string mShardId;
    std::string mCursor;
    std::string mFilter;
    std::string mSubId;
    int mLimit;
};

class GetPBRecordRequest : public GetRecordRequest
{
public:
    GetPBRecordRequest(const std::string& project, const std::string& topic,
                       const std::string& shardId, const std::string& cursor, int limit);

    virtual ~GetPBRecordRequest();

    virtual StringMap GetRequestSpecificHeaders() const;

    virtual std::string SerializePayload() const;

    virtual uint32_t GetCompressMode() const { return ALLOW_RECV_COMPRESS; }
};

class ListConnectorRequest : public DatahubRequest
{
public:
    ListConnectorRequest(
            const std::string& project,
            const std::string& topic);

    virtual ~ListConnectorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

    virtual StringMap GetRequestParameters() const;

private:
    std::string mProject;
    std::string mTopic;
    /**
     * id or type
     */
    std::string mMode;
};

class CreateConnectorRequest : public DatahubRequest
{
public:
    CreateConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const sdk::ConnectorType& type,
            int64_t sinkStartTime,
            const StringVec& columnFields,
            const sdk::SinkConfig& config);

    CreateConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const sdk::ConnectorType& type,
            const StringVec& columnFields,
            const sdk::SinkConfig& config);

    virtual ~CreateConnectorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    sdk::ConnectorType mType;
    int64_t mSinkStartTime;
    StringVec mColumnFields;
    const sdk::SinkConfig *mConfig;
};

class GetConnectorRequest : public DatahubRequest
{
public:
    GetConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual ~GetConnectorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
};

class UpdateConnectorRequest : public DatahubRequest
{
public:
    UpdateConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::SinkConfig& config);

    virtual ~UpdateConnectorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
    const sdk::SinkConfig *mConfig;
};

class DeleteConnectorRequest : public DatahubRequest
{
public:
    DeleteConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual ~DeleteConnectorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
};

class GetConnectorDoneTimeRequest : public DatahubRequest
{
public:
    GetConnectorDoneTimeRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual ~GetConnectorDoneTimeRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
};

class ReloadConnectorRequest : public DatahubRequest
{
public:
    ReloadConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    ReloadConnectorRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId);

    virtual ~ReloadConnectorRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
    /**
     * Reload the entire connector without setting shardId
     */
    std::string mShardId;
};

class UpdateConnectorStateRequest : public DatahubRequest
{
public:
    UpdateConnectorStateRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::ConnectorState& state);

    virtual ~UpdateConnectorStateRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
    sdk::ConnectorState mState;
};

class UpdateConnectorOffsetRequest : public DatahubRequest
{
public:
    UpdateConnectorOffsetRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const sdk::ConnectorOffset& connectorOffset);

    UpdateConnectorOffsetRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId,
            const sdk::ConnectorOffset& connectorOffset);

    virtual ~UpdateConnectorOffsetRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
    /**
     * The id of the shard. If shardId is null, then update all shards offset
     */
    std::string mShardId;
    sdk::ConnectorOffset mConnectorOffset;
};

class GetConnectorShardStatusRequest : public DatahubRequest
{
public:
    GetConnectorShardStatusRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId);

    virtual ~GetConnectorShardStatusRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
};

class GetConnectorShardStatusByShardRequest : public DatahubRequest
{
public:
    GetConnectorShardStatusByShardRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& shardId);

    virtual ~GetConnectorShardStatusByShardRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
    std::string mShardId;
};

class AppendConnectorFieldRequest : public DatahubRequest
{
public:
    AppendConnectorFieldRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& connectorId,
            const std::string& fieldName);

    virtual ~AppendConnectorFieldRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConnectorId;
    std::string mAction;
    std::string mFieldName;
};

class ListSubscriptionRequest : public DatahubRequest
{
public:
    ListSubscriptionRequest(
            const std::string& project,
            const std::string& topic,
            int pageIndex,
            int pageSize,
            const std::string& search);

    ListSubscriptionRequest(
            const std::string& project,
            const std::string& topic,
            int pageIndex,
            int pageSize);

    virtual ~ListSubscriptionRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    int mPageIndex;
    int mPageSize;
    std::string mSearch;
};

class CreateSubscriptionRequest : public DatahubRequest
{
public:
    CreateSubscriptionRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& comment);

    virtual ~CreateSubscriptionRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    std::string mComment;
};

class DeleteSubscriptionRequest : public DatahubRequest
{
public:
    DeleteSubscriptionRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId);

    virtual ~DeleteSubscriptionRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
};

class GetSubscriptionRequest : public DatahubRequest
{
public:
    GetSubscriptionRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId);

    virtual ~GetSubscriptionRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
};

class UpdateSubscriptionRequest : public DatahubRequest
{
public:
    UpdateSubscriptionRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::string& comment);

    virtual ~UpdateSubscriptionRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
    std::string mComment;
};

class UpdateSubscriptionStateRequest : public DatahubRequest
{
public:
    UpdateSubscriptionStateRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const SubscriptionState& state);

    virtual ~UpdateSubscriptionStateRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
    SubscriptionState mState;
};

class OpenSubscriptionOffsetSessionRequest : public DatahubRequest
{
public:
    OpenSubscriptionOffsetSessionRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const StringVec& shardIds);

    virtual ~OpenSubscriptionOffsetSessionRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
    std::string mAction;
    StringVec mShardIds;
};

class GetSubscriptionOffsetRequest : public DatahubRequest
{
public:
    GetSubscriptionOffsetRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const StringVec& shardIds);

    virtual ~GetSubscriptionOffsetRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
    std::string mAction;
    StringVec mShardIds;
};

class UpdateSubscriptionOffsetRequest : public DatahubRequest
{
public:
    UpdateSubscriptionOffsetRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::map<std::string, SubscriptionOffset>& offsets);

    virtual ~UpdateSubscriptionOffsetRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
    std::string mAction;
    std::map<std::string, SubscriptionOffset> mOffsets;
};

class ResetSubscriptionOffsetRequest : public DatahubRequest
{
public:
    ResetSubscriptionOffsetRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& subId,
            const std::map<std::string, SubscriptionOffset>& offsets);

    virtual ~ResetSubscriptionOffsetRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mSubId;
    std::string mAction;
    std::map<std::string, SubscriptionOffset> mOffsets;
};

class HeartbeatRequest : public DatahubRequest
{
public:
    HeartbeatRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId,
            const StringVec& holdShardList,
            const StringVec& readEndShardList);

    virtual ~HeartbeatRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConsumerGroup;
    std::string mAction;
    std::string mConsumerId;
    int64_t mVersionId;
    StringVec mHoldShardList;
    StringVec mReadEndShardList;
};

class JoinGroupRequest : public DatahubRequest
{
public:
    JoinGroupRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            int64_t sessionTimeout);

    virtual ~JoinGroupRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConsumerGroup;
    std::string mAction;
    int64_t mSessionTimeout;
};


class SyncGroupRequest : public DatahubRequest
{
public:
    SyncGroupRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId,
            const StringVec& releaseShardList,
            const StringVec& readEndShardList);

    virtual ~SyncGroupRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConsumerGroup;
    std::string mAction;
    std::string mConsumerId;
    int64_t mVersionId;
    StringVec mReleaseShardList;
    StringVec mReadEndShardList;
};

class LeaveGroupRequest : public DatahubRequest
{
public:
    LeaveGroupRequest(
            const std::string& project,
            const std::string& topic,
            const std::string& consumerGroup,
            const std::string& consumerId,
            int64_t versionId);

    virtual ~LeaveGroupRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mConsumerGroup;
    std::string mAction;
    std::string mConsumerId;
    int64_t mVersionId;
};

class RegisterTopicSchemaRequest : public DatahubRequest
{
public:
    RegisterTopicSchemaRequest(
        const std::string& project,
        const std::string& topic,
        const RecordSchema& schema);

    virtual ~RegisterTopicSchemaRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    RecordSchema mRecordSchema;
};

class ListTopicSchemaRequest : public DatahubRequest
{
public:
    ListTopicSchemaRequest(
        const std::string& project,
        const std::string& topic,
        int pageNumber,
        int pageSize);

    virtual ~ListTopicSchemaRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    int mPageNumber;
    int mPageSize;
};

class GetTopicSchemaRequest : public DatahubRequest
{
public:
    GetTopicSchemaRequest(
        const std::string& project,
        const std::string& topic,
        int64_t versionId,
        const RecordSchema& schema);

    virtual ~GetTopicSchemaRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    int64_t mVersionId;
    RecordSchema mRecordSchema;
};

class DeleteTopicSchemaRequest : public DatahubRequest
{
public:
    DeleteTopicSchemaRequest(
        const std::string& project,
        const std::string& topic,
        int64_t versionId);

    virtual ~DeleteTopicSchemaRequest();

    virtual std::string BuildPath() const;

    virtual std::string SerializePayload() const;

private:
    std::string mProject;
    std::string mTopic;
    std::string mAction;
    int64_t mVersionId;
};

} // namespace datahub
} // namespace aliyun
#endif
