#ifndef DATAHUB_SDK_DATAHUB_RESULT_H
#define DATAHUB_SDK_DATAHUB_RESULT_H

#include <cstdint>
#include <string>
#include <memory>
#include "datahub/datahub_record.h"
#include "datahub/datahub_shard.h"
#include "datahub/datahub_subscription.h"
#include "datahub/datahub_subscription_offset.h"
#include "datahub/datahub_connector.h"
#include "datahub/datahub_typedef.h"
#include "datahub/datahub_meter.h"

namespace aliyun
{
namespace datahub
{

class ErrorEntry
{
public:
    ErrorEntry();
    ~ErrorEntry();

    int GetIndex() const;
    std::string GetErrorCode() const;
    std::string GetErrorMessage() const;
    std::string GetErrorDetail() const;

private:
    friend class JsonTool;
    friend class ProtobufTool;
    int mIndex;
    std::string mErrorCode;
    std::string mErrorMessage;
    std::string mErrorDetail;
};

struct DatahubMetric
{
    uint64_t records;
    uint64_t requests;
    uint64_t rawBytes;
    uint64_t transBytes;
};

class DatahubResult
{
public:
    DatahubResult() {}
    virtual ~DatahubResult() {}

    virtual void DeserializePayload(const std::string& payload) = 0;

    std::string GetRequestId() const { return mRequestId; }
    void SetRequestId(const std::string& requestId) { mRequestId = requestId; }

protected:
    std::string mRequestId;
};

class ListProjectResult : public DatahubResult
{
public:
    ListProjectResult();
    virtual ~ListProjectResult();

    virtual void DeserializePayload(const std::string& payload);

    const StringVec GetProjectNames() const;

private:
    StringVec mProjectNames;
};

class CreateProjectResult : public DatahubResult
{
public:
    CreateProjectResult() {}
    virtual ~CreateProjectResult() {}

    virtual void DeserializePayload(const std::string& payload);
};

class DeleteProjectResult : public DatahubResult
{
public:
    DeleteProjectResult() {}
    virtual ~DeleteProjectResult() {}

    virtual void DeserializePayload(const std::string& payload);
};

class GetProjectResult : public DatahubResult
{
public:
    explicit GetProjectResult(const std::string& project);
    virtual ~GetProjectResult();

    virtual void DeserializePayload(const std::string& payload);

    std::string GetProject() const;
    std::string GetComment() const;
    std::string GetCreator() const;
    int64_t GetCreateTime() const;
    int64_t GetLastModifyTime() const;

private:
    std::string mProject;
    std::string mComment;
    std::string mCreator;
    int64_t mCreateTime;
    int64_t mLastModifyTime;
};

class UpdateProjectResult : public DatahubResult
{
public:
    UpdateProjectResult();
    virtual ~UpdateProjectResult();

    virtual void DeserializePayload(const std::string& payload);
};

class ListTopicResult : public DatahubResult
{
public:
    ListTopicResult();
    virtual ~ListTopicResult();

    virtual void DeserializePayload(const std::string& payload);

    const StringVec GetTopicNames() const;

private:
    StringVec mTopicNames;
};

class CreateTopicResult : public DatahubResult
{
public:
    CreateTopicResult() {}
    virtual ~CreateTopicResult() {}

    virtual void DeserializePayload(const std::string& payload);
};

class DeleteTopicResult : public DatahubResult
{
public:
    DeleteTopicResult() {}
    virtual ~DeleteTopicResult() {}

    virtual void DeserializePayload(const std::string& payload);
};

class UpdateTopicResult : public DatahubResult
{
public:
    UpdateTopicResult();
    virtual ~UpdateTopicResult();

    virtual void DeserializePayload(const std::string& payload);
};

class GetTopicResult : public DatahubResult
{
public:
    GetTopicResult();
    virtual ~GetTopicResult();

    virtual void DeserializePayload(const std::string& payload);

    int GetShardCount() const;
    int GetLifecycle() const;
    std::string GetRecordType() const;
    const RecordSchema& GetRecordSchema() const;
    std::string GetComment() const;
    int64_t GetCreateTime() const;
    int64_t GetLastModifyTime() const;

private:
    int mShardCount;
    int mLifecycle;
    std::string mRecordType;
    RecordSchema mRecordSchema;
    std::string mComment;
    int64_t mCreateTime;
    int64_t mLastModifyTime;
};

class AppendFieldResult : public DatahubResult
{
public:
    AppendFieldResult();
    virtual ~AppendFieldResult();

    virtual void DeserializePayload(const std::string& payload);
};

class ListShardResult : public DatahubResult
{
public:
    ListShardResult();
    virtual ~ListShardResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::vector<ShardEntry>& GetShards() const;

private:
    std::vector<ShardEntry> mShards;
};

class ActivateShardResult : public DatahubResult
{
public:
    ActivateShardResult() {};
    virtual ~ActivateShardResult() {};

    virtual void DeserializePayload(const std::string& payload);
};

class SplitShardResult : public DatahubResult
{
public:
    SplitShardResult();
    virtual ~SplitShardResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::vector<ShardDesc>& GetChildShards() const;

private:
    std::vector<ShardDesc> mShards;
};

class MergeShardResult : public DatahubResult
{
public:
    MergeShardResult();
    virtual ~MergeShardResult();

    virtual void DeserializePayload(const std::string& payload);

    const ShardDesc& GetChildShard() const;

private:
    ShardDesc mShard;
};

class GetMeteringInfoResult : public DatahubResult
{
public:
    GetMeteringInfoResult();
    virtual ~GetMeteringInfoResult();

    virtual void DeserializePayload(const std::string& payload);

    int64_t GetActiveTime() const;

    int64_t GetStorage() const;

private:
    int64_t mActiveTime;
    int64_t mStorage;
};

class GetTopicMeteringInfoResult : public DatahubResult
{
public:
    GetTopicMeteringInfoResult();
    virtual ~GetTopicMeteringInfoResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::vector<MeterRecord>& GetMeterData() const;

private:
    std::vector<MeterRecord> meterData;
};

class PutRecordResult : public DatahubResult
{
public:
    PutRecordResult();
    virtual ~PutRecordResult();

    int GetFailedRecordCount() const;

    const std::vector<ErrorEntry>& GetErrorEntries() const;

    void SetFailedRecords(const std::vector<RecordEntry>& failedRecords);

    void SetFailedRecords(std::vector<RecordEntry>&& failedRecords);

    const std::vector<RecordEntry>& GetFailedRecords() const;

    virtual void DeserializePayload(const std::string& payload);

protected:
    int mFailedRecordCount;
    std::vector<ErrorEntry> mErrorEntries;
    std::vector<RecordEntry> mFailedRecords;
};

class PutPBRecordResult : public PutRecordResult
{
public:
    PutPBRecordResult();
    virtual ~PutPBRecordResult();

    virtual void DeserializePayload(const std::string& payload);
};

class PutRecordByShardResult : public DatahubResult
{
public:
    PutRecordByShardResult() {}
    virtual ~PutRecordByShardResult() {}

    virtual void DeserializePayload(const std::string& payload);
};

class GetCursorResult : public DatahubResult
{
public:
    GetCursorResult();
    virtual ~GetCursorResult();

    virtual void DeserializePayload(const std::string& payload);

    std::string GetCursor() const;
    int64_t GetTimestamp() const;
    int64_t GetSequence() const;
private:
    /**
     * Cursor of the data. Then can read data with this cursor.
     */
    std::string mCursor;
    /**
     * Record time of the data. This is the data writing time.
     */
    int64_t mTimestamp;
    /**
     * Sequence of the data. Each data written in datahub has a sequence.
     */
    int64_t mSequence;
};

class GetRecordResult : public DatahubResult
{
public:
    GetRecordResult();
    virtual ~GetRecordResult();

    std::string GetNextCursor() const;

    int GetRecordCount() const;

    const RecordResult& GetRecord(int index) const;

    const std::vector<RecordResult>& GetRecords() const;

    virtual void DeserializePayload(const std::string& payload);

protected:
    std::string mNextCursor;
    int mRecordCount;
    std::vector<RecordResult> mRecords;
};

class GetPBRecordResult : public GetRecordResult
{
public:
    GetPBRecordResult();
    virtual ~GetPBRecordResult();

    virtual void DeserializePayload(const std::string& payload);
};

class ListConnectorResult : public DatahubResult
{
public:
    ListConnectorResult();
    virtual ~ListConnectorResult();

    virtual void DeserializePayload(const std::string& payload);

    const StringVec GetConnectorIds() const;

private:
    StringVec mConnectorIds;
};

class CreateConnectorResult : public DatahubResult
{
public:
    CreateConnectorResult();
    virtual ~CreateConnectorResult();

    virtual void DeserializePayload(const std::string& payload);

    std::string GetConnectorId() const;

private:
    std::string mConnectorId;
};

class GetConnectorResult : public DatahubResult
{
public:
    GetConnectorResult();
    virtual ~GetConnectorResult();

    virtual void DeserializePayload(const std::string& payload);

    std::string GetClusterAddr() const;

    std::string GetConnectorId() const;

    sdk::ConnectorType GetType() const;

    sdk::ConnectorState GetState() const;

    StringVec GetColumnFields() const;

    std::string GetCreator() const;

    int64_t GetCreateTime() const;

    int64_t GetLastModifyTime() const;

    const sdk::SinkConfig* GetConfig() const;

    StringMap GetExtraConfig() const;

    std::string GetSubId() const;

private:
    std::string mClusterAddr;
    std::string mConnectorId;
    sdk::ConnectorType mType;
    sdk::ConnectorState mState;
    StringVec mColumnFields;
    std::string mCreator;
    int64_t mCreateTime;
    int64_t mLastModifyTime;
    std::shared_ptr<sdk::SinkConfig> mConfig;
    StringMap mExtraConfig;
    std::string mSubId;
};

class UpdateConnectorResult : public DatahubResult
{
public:
    UpdateConnectorResult();
    virtual ~UpdateConnectorResult();

    virtual void DeserializePayload(const std::string& payload);
};

class DeleteConnectorResult : public DatahubResult
{
public:
    DeleteConnectorResult();
    virtual ~DeleteConnectorResult();

    virtual void DeserializePayload(const std::string& payload);
};

class GetConnectorDoneTimeResult : public DatahubResult
{
public:
    GetConnectorDoneTimeResult();
    virtual ~GetConnectorDoneTimeResult();

    virtual void DeserializePayload(const std::string& payload);

    int64_t GetDoneTime() const;

    std::string GetTimeZone() const;

private:
    int64_t mDoneTime;
    std::string mTimeZone;
};

class ReloadConnectorResult : public DatahubResult
{
public:
    ReloadConnectorResult();
    virtual ~ReloadConnectorResult();

    virtual void DeserializePayload(const std::string& payload);
};

class UpdateConnectorStateResult : public DatahubResult
{
public:
    UpdateConnectorStateResult();
    virtual ~UpdateConnectorStateResult();

    virtual void DeserializePayload(const std::string& payload);
};

class UpdateConnectorOffsetResult : public DatahubResult
{
public:
    UpdateConnectorOffsetResult();
    virtual ~UpdateConnectorOffsetResult();

    virtual void DeserializePayload(const std::string& payload);
};

class GetConnectorShardStatusResult : public DatahubResult
{
public:
    GetConnectorShardStatusResult();
    virtual ~GetConnectorShardStatusResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::map<std::string, sdk::ConnectorShardStatusEntry>& GetStatusEntryMap() const;

private:
    std::map<std::string, sdk::ConnectorShardStatusEntry> mStatusEntryMap;
};

class GetConnectorShardStatusByShardResult : public DatahubResult
{
public:
    GetConnectorShardStatusByShardResult();
    virtual ~GetConnectorShardStatusByShardResult();

    virtual void DeserializePayload(const std::string& payload);

    const sdk::ConnectorShardStatusEntry& GetStatusEntry() const;

private:
    sdk::ConnectorShardStatusEntry mStatusEntry;
};

class AppendConnectorFieldResult : public DatahubResult
{
public:
    AppendConnectorFieldResult();
    virtual ~AppendConnectorFieldResult();

    virtual void DeserializePayload(const std::string& payload);
};


class ListSubscriptionResult : public DatahubResult
{
public:
    ListSubscriptionResult();
    virtual ~ListSubscriptionResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::vector<SubscriptionEntry>& GetSubscriptions() const;
    int64_t GetTotalCount() const;

private:
    std::vector<SubscriptionEntry> mSubscriptions;
    int64_t mTotalCount;
};

class CreateSubscriptionResult : public DatahubResult
{
public:
    CreateSubscriptionResult();

    virtual ~CreateSubscriptionResult();

    std::string GetSubId() const;

    virtual void DeserializePayload(const std::string& payload);

private:
    std::string mSubId;
};

class DeleteSubscriptionResult : public DatahubResult
{
public:
    DeleteSubscriptionResult();
    virtual ~DeleteSubscriptionResult();

    virtual void DeserializePayload(const std::string& payload);
};

class GetSubscriptionResult : public DatahubResult
{
public:
    GetSubscriptionResult();
    virtual ~GetSubscriptionResult();

    virtual void DeserializePayload(const std::string& payload);

    std::string GetSubId() const;
    std::string GetComment() const;
    bool GetIsOwner() const;
    SubscriptionType GetType() const;
    SubscriptionState GetState() const;
    int64_t GetCreateTime() const;
    int64_t GetLastModifyTime() const;

private:
    SubscriptionEntry subscriptionEntry;
};

class UpdateSubscriptionResult : public DatahubResult
{
public:
    UpdateSubscriptionResult();
    virtual ~UpdateSubscriptionResult();

    virtual void DeserializePayload(const std::string& payload);
};

class UpdateSubscriptionStateResult : public DatahubResult
{
public:
    UpdateSubscriptionStateResult();
    virtual ~UpdateSubscriptionStateResult();

    virtual void DeserializePayload(const std::string& payload);
};

class OpenSubscriptionOffsetSessionResult : public DatahubResult
{
public:
    OpenSubscriptionOffsetSessionResult();

    virtual ~OpenSubscriptionOffsetSessionResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::map<std::string, SubscriptionOffset>& GetOffsets() const;

private:
    std::map<std::string, SubscriptionOffset> mOffsets;
};

class GetSubscriptionOffsetResult : public DatahubResult
{
public:
    GetSubscriptionOffsetResult();

    virtual ~GetSubscriptionOffsetResult();

    virtual void DeserializePayload(const std::string& payload);

    const std::map<std::string, SubscriptionOffset>& GetOffsets() const;

private:
    std::map<std::string, SubscriptionOffset> mOffsets;
};

class UpdateSubscriptionOffsetResult : public DatahubResult
{
public:
    UpdateSubscriptionOffsetResult();

    virtual ~UpdateSubscriptionOffsetResult();

    virtual void DeserializePayload(const std::string& payload);
};

class ResetSubscriptionOffsetResult : public DatahubResult
{
public:
    ResetSubscriptionOffsetResult();

    virtual ~ResetSubscriptionOffsetResult();

    virtual void DeserializePayload(const std::string& payload);
};

class HeartbeatResult : public DatahubResult
{
public:
    HeartbeatResult();
    virtual ~HeartbeatResult();

    virtual void DeserializePayload(const std::string& payload);

    int64_t GetPlanVersion() const;
    const StringVec GetShardList() const;
    std::string GetTotalPlan() const;

private:
    int64_t mPlanVersion;
    StringVec mShardList;
    std::string mTotalPlan;
};

class JoinGroupResult : public DatahubResult
{
public:
    JoinGroupResult();
    virtual ~JoinGroupResult();

    virtual void DeserializePayload(const std::string& payload);

    std::string GetConsumerId() const;
    int64_t GetVersionId() const;
    int64_t GetSessionTimeout() const;

private:
    std::string mConsumerId;
    int64_t mVersionId;
    int64_t mSessionTimeout;
};

class SyncGroupResult : public DatahubResult
{
public:
    SyncGroupResult();

    virtual ~SyncGroupResult();

    virtual void DeserializePayload(const std::string& payload);
};

class LeaveGroupResult : public DatahubResult
{
public:
    LeaveGroupResult();

    virtual ~LeaveGroupResult();

    virtual void DeserializePayload(const std::string& payload);
};

class RegisterTopicSchemaResult : public DatahubResult
{
public:
    RegisterTopicSchemaResult();

    virtual ~RegisterTopicSchemaResult();

    virtual void DeserializePayload(const std::string& payload);

    int64_t GetVersionId() const;

private:
    int64_t mVersionId;
};

class ListTopicSchemaResult : public DatahubResult
{
public:
    ListTopicSchemaResult();

    virtual ~ListTopicSchemaResult();

    virtual void DeserializePayload(const std::string& payload);

    int64_t GetPageNumber() const;

    int64_t GetPageSize() const;

    int64_t GetPageCount() const;

    int64_t GetTotalCount() const;

    const std::vector<RecordSchema>& GetRecordSchemaList() const;

private:
    int64_t mPageNumber;
    int64_t mPageSize;
    int64_t mPageCount;
    int64_t mTotalCount;
    std::vector<RecordSchema> mRecordSchemaList;
};

class GetTopicSchemaResult : public DatahubResult
{
public:
    GetTopicSchemaResult();

    virtual ~GetTopicSchemaResult();

    virtual void DeserializePayload(const std::string& payload);

    int64_t GetVersionId() const;

    int64_t GetCreateTime() const;

    std::string GetCreator() const;

    const RecordSchema& GetRecordSchema() const;

private:
    int64_t mVersionId;
    int64_t mCreateTime;
    std::string mCreator;
    RecordSchema mRecordSchema;
};

class DeleteTopicSchemaResult : public DatahubResult
{
public:
    DeleteTopicSchemaResult();

    virtual ~DeleteTopicSchemaResult();

    virtual void DeserializePayload(const std::string& payload);
};

} // namespace datahub
} // namespace aliyun
#endif
