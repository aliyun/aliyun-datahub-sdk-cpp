#ifndef DATAHUB_DATAHUB_CONNECTOR_H
#define DATAHUB_DATAHUB_CONNECTOR_H

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>
#include "rapidjson/document.h"
#include "datahub/datahub_typedef.h"

namespace aliyun
{
namespace datahub
{
namespace sdk
{

enum AuthMode
{
    /**
     * AK mode: you should input accessId and accessKey
     */
    AK = 0,

    /**
     * Service will get a temporary key to visit other services.
     */
    STS = 1
};

std::string GetNameForAuthMode(const AuthMode& mode);
AuthMode GetAuthModeFromName(const std::string& name);

enum ConnectorType
{
    SINK_ODPS,
    SINK_DATAHUB,
    SINK_OSS,
    SINK_ES,
    SINK_ADS,
    SINK_MYSQL,
    SINK_FC,
    SINK_OTS
};

std::string GetNameForConnectorType(const ConnectorType& type);
ConnectorType GetConnectorTypeFromName(const std::string& name);

enum ConnectorState
{
    /**
     * Deprecated
     */
    CONNECTOR_CREATED = 0,
    /**
     * Deprecated
     */
    CONNECTOR_PAUSED = 1,
    CONNECTOR_STOPPED = 2,
    CONNECTOR_RUNNING = 3
};

std::string GetNameForConnectorState(const ConnectorState& state);
ConnectorState GetConnectorStateFromName(const std::string& name);

class SinkConfig
{
public:
    SinkConfig();
    virtual ~SinkConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);
};

class SinkDatahubConfig : public SinkConfig
{
public:
    SinkDatahubConfig();

    virtual ~SinkDatahubConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetEndpoint(const std::string& endpoint);
    std::string GetEndpoint() const;

    void SetProject(const std::string& project);
    std::string GetProject() const;

    void SetTopic(const std::string& topic);
    std::string GetTopic() const;

    void SetAuthMode(const AuthMode& authMode);
    AuthMode GetAuthMode() const;

    void SetAccessId(const std::string& accessId);
    std::string GetAccessId() const;

    void SetAccessKey(const std::string& accessKey);
    std::string GetAccessKey() const;

private:
    /**
     * DataHub service endpoint.
     */
    std::string mEndpoint;
    /**
     * DataHub project name.
     */
    std::string mProject;
    /**
     * Datahub topic name.
     */
    std::string mTopic;
    /**
     * Authentication mode.
     */
    AuthMode mAuthMode;
    /**
     * Datahub accessId.
     */
    std::string mAccessId;
    /**
     * Datahub accessKey.
     */
    std::string mAccessKey;
};

class SinkEsConfig : public SinkConfig
{
public:
    SinkEsConfig();

    virtual ~SinkEsConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetIndex(const std::string& index);
    std::string GetIndex() const;

    void SetEndpoint(const std::string& endpoint);
    std::string GetEndpoint() const;

    void SetUser(const std::string& user);
    std::string GetUser() const;

    void SetPassword(const std::string& password);
    std::string GetPassword() const;

    void SetIdFields(const StringVec& idFields);
    const StringVec& GetIdFields() const;

    void SetTypeFields(const StringVec& typeFields);
    const StringVec& GetTypeFields() const;

    void SetProxyMode(bool proxyMode);
    bool GetProxyMode() const;

private:
    /**
     * Elastic search index
     */
    std::string mIndex;
    /**
     * Elastic search service endpoint
     */
    std::string mEndpoint;
    /**
     * User used to synchronize data
     */
    std::string mUser;
    /**
     * User password to synchronize data
     */
    std::string mPassword;
    /**
     * Id fields to synchronize data
     */
    StringVec mIdFields;
    /**
     * Type fields to synchronize data
     */
    StringVec mTypeFields;

    /**
     * default is true.
     */
    bool mProxyMode;
};

class SinkFcConfig : public SinkConfig
{
public:
    SinkFcConfig();

    virtual ~SinkFcConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetEndpoint(const std::string& endpoint);
    std::string GetEndpoint() const;

    void SetService(const std::string& service);
    std::string GetService() const;

    void SetFunction(const std::string& function);
    std::string GetFunction() const;

    void SetAuthMode(const AuthMode& authMode);
    AuthMode GetAuthMode() const;

    void SetAccessId(const std::string& accessId);
    std::string GetAccessId() const;

    void SetAccessKey(const std::string& accessKey);
    std::string GetAccessKey() const;

private:
    /**
     * The endpoint of the function compute service
     */
    std::string mEndpoint;
    /**
     * The name of the function compute service
     */
    std::string mService;
    /**
     * The function name used to synchronize data
     */
    std::string mFunction;
    /**
     * Authentication mode when visiting fc service
     */
    AuthMode mAuthMode;
    /**
     * AccessId used to visit fc service
     */
    std::string mAccessId;
    /**
     * AccessKey used to visit fc service
     */
    std::string mAccessKey;
};

class SinkOssConfig : public SinkConfig
{
public:
    SinkOssConfig();

    virtual ~SinkOssConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetEndpoint(const std::string& endpoint);
    std::string GetEndpoint() const;

    void SetBucket(const std::string& bucket);
    std::string GetBucket() const;

    void SetPrefix(const std::string& prefix);
    std::string GetPrefix() const;

    void SetTimeFormat(const std::string& timeFormat);
    std::string GetTimeFormat() const;

    void SetTimeRange(int timeRange);
    int GetTimeRange() const;

    void SetAuthMode(const AuthMode& authMode);
    AuthMode GetAuthMode() const;

    void SetAccessId(const std::string& accessId);
    std::string GetAccessId() const;

    void SetAccessKey(const std::string& accessKey);
    std::string GetAccessKey() const;

private:
    /**
     * Endpoint of the oss service
     */
    std::string mEndpoint;
    /**
     * Bucket of the oss service
     */
    std::string mBucket;
    /**
     * Prefix of the oss service
     */
    std::string mPrefix;
    /**
     * Specify how to compute partition
     */
    std::string mTimeFormat;
    /**
     * How long time data that the oss partition to store.
     * <br>
     * Unit: Minute
     */
    int mTimeRange;
    /**
     * Authentication mode when visiting oss service
     */
    AuthMode mAuthMode;
    /**
    * AccessId used to visit oss service
    */
    std::string mAccessId;
    /**
     * AccessKey used to visit oss service
     */
    std::string mAccessKey;
};

class SinkOtsConfig : public SinkConfig
{
public:
    SinkOtsConfig();

    virtual ~SinkOtsConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetEndpoint(const std::string& endpoint);
    std::string GetEndpoint() const;

    void SetInstance(const std::string& instance);
    std::string GetInstance() const;

    void SetTable(const std::string& table);
    std::string GetTable() const;

    void SetAuthMode(const AuthMode& authMode);
    AuthMode GetAuthMode() const;

    void SetAccessId(const std::string& accessId);
    std::string GetAccessId() const;

    void SetAccessKey(const std::string& accessKey);
    std::string GetAccessKey() const;

private:
    /**
     * Endpoint of the ots service
     */
    std::string mEndpoint;
    /**
     * Instance name of the ots service
     */
    std::string mInstance;
    /**
     * table name of the ots service
     */
    std::string mTable;
    /**
     * Authentication mode when visiting ots service
     */
    AuthMode mAuthMode;
    /**
     * AccessId used to visit ots service
     */
    std::string mAccessId;
    /**
     * AccessKey used to visit ots service
     */
    std::string mAccessKey;
};

enum InsertMode {
    /**
     * Insert ignore if the database has the same key record.
     */
    IGNORE = 0,

    /**
     * Insert overwrite if the database has the same key record.
     */
    OVERWRITE = 1,
};

std::string GetNameForInsertMode(const InsertMode& mode);
InsertMode GetInsertModeFromName(const std::string& name);

class SinkMysqlConfig : public SinkConfig
{
public:
    SinkMysqlConfig();

    virtual ~SinkMysqlConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetHost(const std::string& host);
    std::string GetHost() const;

    void SetPort(int port);
    int GetPort() const;

    void SetDatabase(const std::string& database);
    std::string GetDatabase() const;

    void SetTable(const std::string& table);
    std::string GetTable() const;

    void SetUser(const std::string& user);
    std::string GetUser() const;

    void SetPassword(const std::string& password);
    std::string GetPassword() const;

    void SetInsertMode(const InsertMode& insertMode);
    InsertMode GetInsertMode() const;

protected:
    /**
     * The host of the database.
     */
    std::string mHost;
    /**
     * The port of the database.
     */
    int mPort;
    /**
     * Database name used to synchronize data.
     */
    std::string mDatabase;
    /**
     * Table name used to synchronize data.
     */
    std::string mTable;
    /**
     * User name used to synchronize data.
     */
    std::string mUser;
    /**
     * User password used to synchronize data.
     */
    std::string mPassword;
    /**
     * Ignore write or not, default is IGNORE.
     */
    InsertMode mInsertMode;
};

class SinkAdsConfig : public SinkMysqlConfig
{
public:
    SinkAdsConfig();

    virtual ~SinkAdsConfig();
};

enum PartitionMode {
    /**
     * Partitioned by user defined columns.
     */
    USER_DEFINE = 0,
    /**
     * First should have a column named 'event_time TIMESTAMP', and then partitioned by this column value.
     */
    SYSTEM_TIME = 1,
    /**
     * Partitioned by time that a record written into DataHub.
     */
    EVENT_TIME = 2,
    /**
    * Partitioned by time from attributes
    */
    META_TIME = 3
};

std::string GetNameForPartitionMode(const PartitionMode& mode);
PartitionMode GetPartitionModeFromName(const std::string& name);

class SinkOdpsConfig : public SinkConfig
{
public:
    SinkOdpsConfig();

    virtual ~SinkOdpsConfig();

    virtual void SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const;

    virtual void JsonToSinkConfig(const rapidjson::Value& jsonValue);

    void SetEndpoint(const std::string& endpoint);
    std::string GetEndpoint() const;

    void SetProject(const std::string& project);
    std::string GetProject() const;

    void SetTable(const std::string& table);
    std::string GetTable() const;

    void SetTunnelEndpoint(const std::string& tunnelEndpoint);
    std::string GetTunnelEndpoint() const;

    void SetAccessId(const std::string& accessId);
    std::string GetAccessId() const;

    void SetAccessKey(const std::string& accessKey);
    std::string GetAccessKey() const;

    void SetPartitionMode(const PartitionMode& partitionMode);
    PartitionMode GetPartitionMode() const;

    void SetTimeRange(int timeRange);
    int GetTimeRange() const;

    void SetTimeZone(const std::string& timeZone);
    std::string GetTimeZone() const;

    void SetPartitionConfig(const StringPairVec& partitionConfig);
    const StringPairVec& GetPartitionConfig() const;

    void SetSplitKey(const std::string& splitKey);
    std::string GetSplitKey() const;

    void SetBase64Encode(bool base64Encode);
    bool GetBase64Encode() const;

private:
    /**
     * The endpoint of the MaxCompute service.
     */
    std::string mEndpoint;
    /**
     * MaxCompute project name.
     */
    std::string mProject;
    /**
     * MaxCompute table name.
     */
    std::string mTable;
    /**
     * The endpoint of the Tunnel service which is bound with MaxCompute<br>
     * If not specified, DataHub will get tunnel endpoint from MaxCompute project config.
     */
    std::string mTunnelEndpoint;
    /**
     * The accessId to visit MaxCompute. The accessId should have permission to visit the corresponding MaxCompute table.
     */
    std::string mAccessId;
    /**
     * The accessKey to visit MaxCompute.
     */
    std::string mAccessKey;
    /**
     * Specify how to partition data info corresponding MaxCompute partition.
     */
    PartitionMode mPartitionMode;
    /**
     * How long time data that a MaxCompute partition to store. Used in SYSTEM_TIME and EVENT_TIME mode.
     * <br>
     * Unit: Minute
     */
    int mTimeRange;
    /**
     * Timezone for odps table, used for compute partition.
     * If not set, use odps project config or default(Asia/Shanghai).
     */
    std::string mTimeZone;
    /**
     * Specify how to compute MaxCompute partition with the specified column value.
     */
    StringPairVec mPartitionConfig;
    /**
     * Specify splitKey if blob data should split to single lines before sink to ODPS, only for BLOB topic.
     */
    std::string mSplitKey;
    /**
     * Where base64encode before sinked into ODPS, only for BLOB topic.
     * Usually user should not use this parameter, default is false.
     */
    bool mBase64Encode;
};


enum ConnectorShardState
{
    CONTEXT_HANG = 0,
    CONTEXT_PLANNED = 1,
    CONTEXT_EXECUTING = 2,
    /**
     * Deprecated
     */
    CONTEXT_PAUSED = 3,
    CONTEXT_STOPPED = 4,
    CONTEXT_FINISHED = 5
};

std::string GetNameForConnectorShardState(const ConnectorShardState &state);
ConnectorShardState GetConnectorShardStateFromName(const std::string& name);

class ConnectorShardStatusEntry
{
public:
    ConnectorShardStatusEntry();

    virtual ~ConnectorShardStatusEntry();

    int64_t GetStartSequence() const;
    int64_t GetEndSequence() const;
    int64_t GetCurrentSequence() const;
    int64_t GetCurrentTimestamp() const;
    int64_t GetUpdateTime() const;
    ConnectorShardState GetState() const;
    std::string GetLastErrorMessage() const;
    int64_t GetDiscardCount() const;
    int64_t GetDoneTime() const;
    std::string GetWorkerAddress() const;

    void JsonToShardStatusEntry(const rapidjson::Value& jsonValue);

private:
    int64_t mStartSequence;
    int64_t mEndSequence;
    int64_t mCurrentSequence;
    int64_t mCurrentTimestamp;
    int64_t mUpdateTime;
    ConnectorShardState mState;
    std::string mLastErrorMessage;
    int64_t mDiscardCount;
    int64_t mDoneTime;
    std::string mWorkerAddress;
};

class ConnectorOffset
{
public:
    ConnectorOffset();
    ConnectorOffset(
            int64_t timestamp,
            int64_t sequence);
    ~ConnectorOffset();

    int64_t GetTimestamp() const;
    int64_t GetSequence() const;

protected:
    int64_t mTimestamp;
    int64_t mSequence;
};

}
}
}
#endif //DATAHUB_DATAHUB_CONNECTOR_H
