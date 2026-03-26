#include <string>
#include "datahub/datahub_subscription.h"
#include "datahub/datahub_connector.h"
#include "utils.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{
namespace sdk
{

std::string GetNameForAuthMode(const AuthMode& mode)
{
    switch (mode)
    {
        case AK:
            return "AK";
        case STS:
            return "STS";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid auth mode");
    }
}

AuthMode GetAuthModeFromName(const std::string& name)
{
    if (name == "AK")
    {
        return AK;
    }
    else if (name == "STS")
    {
        return STS;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported auth mode " + name);
    }
}

std::string GetNameForConnectorType(const ConnectorType& type)
{
    switch (type)
    {
        case SINK_ODPS:
            return "SINK_ODPS";
        case SINK_DATAHUB:
            return "SINK_DATAHUB";
        case SINK_OSS:
            return "SINK_OSS";
        case SINK_ES:
            return "SINK_ES";
        case SINK_ADS:
            return "SINK_ADS";
        case SINK_MYSQL:
            return "SINK_MYSQL";
        case SINK_FC:
            return "SINK_FC";
        case SINK_OTS:
            return "SINK_OTS";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid connector type");
    }
}

ConnectorType GetConnectorTypeFromName(const std::string& name)
{
    if (name == "SINK_ODPS")
    {
        return SINK_ODPS;
    }
    else if (name == "SINK_DATAHUB")
    {
        return SINK_DATAHUB;
    }
    else if (name == "SINK_OSS")
    {
        return SINK_OSS;
    }
    else if (name == "SINK_ES")
    {
        return SINK_ES;
    }
    else if (name == "SINK_ADS")
    {
        return SINK_ADS;
    }
    else if (name == "SINK_MYSQL")
    {
        return SINK_MYSQL;
    }
    else if (name == "SINK_FC")
    {
        return SINK_FC;
    }
    else if (name == "SINK_OTS")
    {
        return SINK_OTS;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported connector type " + name);
    }
}

std::string GetNameForConnectorState(const ConnectorState& state)
{
    switch (state)
    {
        case CONNECTOR_CREATED:
            return "CONNECTOR_CREATED";
        case CONNECTOR_PAUSED:
            return "CONNECTOR_PAUSED";
        case CONNECTOR_STOPPED:
            return "CONNECTOR_STOPPED";
        case CONNECTOR_RUNNING:
            return "CONNECTOR_RUNNING";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid connector state");
    }
}

ConnectorState GetConnectorStateFromName(const std::string& name)
{
    if (name == "CONNECTOR_CREATED")
    {
        return CONNECTOR_CREATED;
    }
    else if (name == "CONNECTOR_PAUSED")
    {
        return CONNECTOR_PAUSED;
    }
    else if (name == "CONNECTOR_STOPPED")
    {
        return CONNECTOR_STOPPED;
    }
    else if (name == "CONNECTOR_RUNNING")
    {
        return CONNECTOR_RUNNING;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported connector state " + name);
    }
}


SinkConfig::SinkConfig()
{
}

SinkConfig::~SinkConfig()
{
}

void SinkConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
}

void SinkConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
}

SinkDatahubConfig::SinkDatahubConfig() :
        mAuthMode(AuthMode::AK)
{
}

SinkDatahubConfig::~SinkDatahubConfig()
{
}

void SinkDatahubConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value endpoint(rapidjson::kStringType);
    endpoint.SetString(mEndpoint.c_str(), allocator);
    jsonValue.AddMember("Endpoint", endpoint, allocator);

    rapidjson::Value project(rapidjson::kStringType);
    project.SetString(mProject.c_str(), allocator);
    jsonValue.AddMember("Project", project, allocator);

    rapidjson::Value topic(rapidjson::kStringType);
    topic.SetString(mTopic.c_str(), allocator);
    jsonValue.AddMember("Topic", topic, allocator);

    rapidjson::Value authMode(rapidjson::kStringType);
    authMode.SetString(Utils::ToLowerCaseString(GetNameForAuthMode(mAuthMode)).c_str(), allocator);
    jsonValue.AddMember("AuthMode", authMode, allocator);

    if (mAuthMode == AuthMode::AK)
    {
        rapidjson::Value accessId(rapidjson::kStringType);
        accessId.SetString(mAccessId.c_str(), allocator);
        jsonValue.AddMember("AccessId", accessId, allocator);

        rapidjson::Value accessKey(rapidjson::kStringType);
        accessKey.SetString(mAccessKey.c_str(), allocator);
        jsonValue.AddMember("AccessKey", accessKey, allocator);
    }
}

void SinkDatahubConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator endpointItr = jsonValue.FindMember("Endpoint");
    if (endpointItr != jsonValue.MemberEnd() && endpointItr->value.IsString())
    {
        mEndpoint.assign(endpointItr->value.GetString(), endpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator projectItr = jsonValue.FindMember("Project");
    if (projectItr != jsonValue.MemberEnd() && projectItr->value.IsString())
    {
        mProject.assign(projectItr->value.GetString(), projectItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator topicItr = jsonValue.FindMember("Topic");
    if (topicItr != jsonValue.MemberEnd() && topicItr->value.IsString())
    {
        mTopic.assign(topicItr->value.GetString(), topicItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator authModeItr = jsonValue.FindMember("AuthMode");
    if (authModeItr != jsonValue.MemberEnd() && authModeItr->value.IsString())
    {
        std::string str = std::string(authModeItr->value.GetString(), authModeItr->value.GetStringLength());
        if (!str.empty())
        {
            mAuthMode = GetAuthModeFromName(Utils::ToUpperCaseString(str));
        }
    }

    rapidjson::Value::ConstMemberIterator accessIdItr = jsonValue.FindMember("AccessId");
    if (accessIdItr != jsonValue.MemberEnd() && accessIdItr->value.IsString())
    {
        mAccessId.assign(accessIdItr->value.GetString(), accessIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator accessKeyItr = jsonValue.FindMember("AccessKey");
    if (accessKeyItr != jsonValue.MemberEnd() && accessKeyItr->value.IsString())
    {
        mAccessKey.assign(accessKeyItr->value.GetString(), accessKeyItr->value.GetStringLength());
    }
}

void SinkDatahubConfig::SetEndpoint(const std::string& endpoint)
{
    mEndpoint = endpoint;
}

std::string SinkDatahubConfig::GetEndpoint() const
{
    return mEndpoint;
}

void SinkDatahubConfig::SetProject(const std::string& project)
{
    mProject = project;
}

std::string SinkDatahubConfig::GetProject() const
{
    return mProject;
}

void SinkDatahubConfig::SetTopic(const std::string& topic)
{
    mTopic = topic;
}

std::string SinkDatahubConfig::GetTopic() const
{
    return mTopic;
}

void SinkDatahubConfig::SetAuthMode(const AuthMode& authMode)
{
    mAuthMode = authMode;
}

AuthMode SinkDatahubConfig::GetAuthMode() const
{
    return mAuthMode;
}

void SinkDatahubConfig::SetAccessId(const std::string& accessId)
{
    mAccessId = accessId;
}

std::string SinkDatahubConfig::GetAccessId() const
{
    return mAccessId;
}

void SinkDatahubConfig::SetAccessKey(const std::string& accessKey)
{
    mAccessKey = accessKey;
}

std::string SinkDatahubConfig::GetAccessKey() const
{
    return mAccessKey;
}


SinkEsConfig::SinkEsConfig() :
        mProxyMode(true)
{
}

SinkEsConfig::~SinkEsConfig()
{
}

void SinkEsConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value index(rapidjson::kStringType);
    index.SetString(mIndex.c_str(), allocator);
    jsonValue.AddMember("Index", index, allocator);

    rapidjson::Value endpoint(rapidjson::kStringType);
    endpoint.SetString(mEndpoint.c_str(), allocator);
    jsonValue.AddMember("Endpoint", endpoint, allocator);

    rapidjson::Value user(rapidjson::kStringType);
    user.SetString(mUser.c_str(), allocator);
    jsonValue.AddMember("User", user, allocator);

    rapidjson::Value password(rapidjson::kStringType);
    password.SetString(mPassword.c_str(), allocator);
    jsonValue.AddMember("Password", password, allocator);

    rapidjson::Value idFields(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator idItr;
    for (idItr = mIdFields.begin(); idItr != mIdFields.end(); ++idItr)
    {
        rapidjson::Value idField(rapidjson::kStringType);
        idField.SetString(idItr->c_str(), idItr->size(), allocator);
        idFields.PushBack(idField, allocator);
    }
    jsonValue.AddMember("IDFields", idFields, allocator);

    rapidjson::Value typeFields(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator typeItr;
    for (typeItr = mTypeFields.begin(); typeItr != mTypeFields.end(); ++typeItr)
    {
        rapidjson::Value typeField(rapidjson::kStringType);
        typeField.SetString(typeItr->c_str(), typeItr->size(), allocator);
        typeFields.PushBack(typeField, allocator);
    }
    jsonValue.AddMember("TypeFields", typeFields, allocator);

    rapidjson::Value proxyMode(rapidjson::kStringType);
    proxyMode.SetString(Utils::BoolToString(mProxyMode).c_str(), allocator);
    jsonValue.AddMember("ProxyMode", proxyMode, allocator);
}

void SinkEsConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator indexItr = jsonValue.FindMember("Index");
    if (indexItr != jsonValue.MemberEnd() && indexItr->value.IsString())
    {
        mIndex.assign(indexItr->value.GetString(), indexItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator endpointItr = jsonValue.FindMember("Endpoint");
    if (endpointItr != jsonValue.MemberEnd() && endpointItr->value.IsString())
    {
        mEndpoint.assign(endpointItr->value.GetString(), endpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator userItr = jsonValue.FindMember("User");
    if (userItr != jsonValue.MemberEnd() && userItr->value.IsString())
    {
        mUser.assign(userItr->value.GetString(), userItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator passwordItr = jsonValue.FindMember("Password");
    if (passwordItr != jsonValue.MemberEnd() && passwordItr->value.IsString())
    {
        mPassword.assign(passwordItr->value.GetString(), passwordItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator idItr = jsonValue.FindMember("IDFields");
    if (idItr != jsonValue.MemberEnd() && idItr->value.IsArray())
    {
        const rapidjson::Value& idFields = idItr->value;
        for (rapidjson::SizeType i = 0; i < idFields.Size(); ++i)
        {
            std::string idField = idFields[i].GetString();
            mIdFields.push_back(idField);
        }
    }

    rapidjson::Value::ConstMemberIterator tfItr = jsonValue.FindMember("TypeFields");
    if (tfItr != jsonValue.MemberEnd() && tfItr->value.IsArray())
    {
        const rapidjson::Value& typeFields = tfItr->value;
        for (rapidjson::SizeType i = 0; i < typeFields.Size(); ++i)
        {
            std::string typeField = typeFields[i].GetString();
            mTypeFields.push_back(typeField);
        }
    }

    rapidjson::Value::ConstMemberIterator connectorIdItr = jsonValue.FindMember("ProxyMode");
    if (connectorIdItr != jsonValue.MemberEnd() && connectorIdItr->value.IsString())
    {
        mProxyMode = Utils::StringToBool(connectorIdItr->value.GetString());
    }
}

void SinkEsConfig::SetIndex(const std::string& index)
{
    mIndex = index;
}

std::string SinkEsConfig::GetIndex() const
{
    return mIndex;
}

void SinkEsConfig::SetEndpoint(const std::string& endpoint)
{
    mEndpoint = endpoint;
}

std::string SinkEsConfig::GetEndpoint() const
{
    return mEndpoint;
}

void SinkEsConfig::SetUser(const std::string& user)
{
    mUser = user;
}

std::string SinkEsConfig::GetUser() const
{
    return mUser;
}

void SinkEsConfig::SetPassword(const std::string& password)
{
    mPassword = password;
}

std::string SinkEsConfig::GetPassword() const
{
    return mPassword;
}

void SinkEsConfig::SetIdFields(const StringVec& idFields)
{
    mIdFields = idFields;
}

const StringVec& SinkEsConfig::GetIdFields() const
{
    return mIdFields;
}

void SinkEsConfig::SetTypeFields(const StringVec& typeFields)
{
    mTypeFields = typeFields;
}

const StringVec& SinkEsConfig::GetTypeFields() const
{
    return mTypeFields;
}

void SinkEsConfig::SetProxyMode(bool proxyMode)
{
    mProxyMode = proxyMode;
}

bool SinkEsConfig::GetProxyMode() const
{
    return mProxyMode;
}


SinkFcConfig::SinkFcConfig() :
        mAuthMode(AuthMode::AK)
{
}

SinkFcConfig::~SinkFcConfig()
{
}

void SinkFcConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value endpoint(rapidjson::kStringType);
    endpoint.SetString(mEndpoint.c_str(), allocator);
    jsonValue.AddMember("Endpoint", endpoint, allocator);

    rapidjson::Value service(rapidjson::kStringType);
    service.SetString(mService.c_str(), allocator);
    jsonValue.AddMember("Service", service, allocator);

    rapidjson::Value function(rapidjson::kStringType);
    function.SetString(mFunction.c_str(), allocator);
    jsonValue.AddMember("Function", function, allocator);

    rapidjson::Value authMode(rapidjson::kStringType);
    authMode.SetString(Utils::ToLowerCaseString(GetNameForAuthMode(mAuthMode)).c_str(), allocator);
    jsonValue.AddMember("AuthMode", authMode, allocator);

    if (mAuthMode == AuthMode::AK)
    {
        rapidjson::Value accessId(rapidjson::kStringType);
        accessId.SetString(mAccessId.c_str(), allocator);
        jsonValue.AddMember("AccessId", accessId, allocator);

        rapidjson::Value accessKey(rapidjson::kStringType);
        accessKey.SetString(mAccessKey.c_str(), allocator);
        jsonValue.AddMember("AccessKey", accessKey, allocator);
    }
}

void SinkFcConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator endpointItr = jsonValue.FindMember("Endpoint");
    if (endpointItr != jsonValue.MemberEnd() && endpointItr->value.IsString())
    {
        mEndpoint.assign(endpointItr->value.GetString(), endpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator serviceItr = jsonValue.FindMember("Service");
    if (serviceItr != jsonValue.MemberEnd() && serviceItr->value.IsString())
    {
        mService.assign(serviceItr->value.GetString(), serviceItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator functionItr = jsonValue.FindMember("Function");
    if (functionItr != jsonValue.MemberEnd() && functionItr->value.IsString())
    {
        mFunction.assign(functionItr->value.GetString(), functionItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator authModeItr = jsonValue.FindMember("AuthMode");
    if (authModeItr != jsonValue.MemberEnd() && authModeItr->value.IsString())
    {
        std::string str = std::string(authModeItr->value.GetString(), authModeItr->value.GetStringLength());
        if (!str.empty())
        {
            mAuthMode = GetAuthModeFromName(Utils::ToUpperCaseString(str));
        }
    }

    rapidjson::Value::ConstMemberIterator accessIdItr = jsonValue.FindMember("AccessId");
    if (accessIdItr != jsonValue.MemberEnd() && accessIdItr->value.IsString())
    {
        mAccessId.assign(accessIdItr->value.GetString(), accessIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator accessKeyItr = jsonValue.FindMember("AccessKey");
    if (accessKeyItr != jsonValue.MemberEnd() && accessKeyItr->value.IsString())
    {
        mAccessKey.assign(accessKeyItr->value.GetString(), accessKeyItr->value.GetStringLength());
    }
}

void SinkFcConfig::SetEndpoint(const std::string& endpoint)
{
    mEndpoint = endpoint;
}

std::string SinkFcConfig::GetEndpoint() const
{
    return mEndpoint;
}

void SinkFcConfig::SetService(const std::string& service)
{
    mService = service;
}

std::string SinkFcConfig::GetService() const
{
    return mService;
}

void SinkFcConfig::SetFunction(const std::string& function)
{
    mFunction = function;
}

std::string SinkFcConfig::GetFunction() const
{
    return mFunction;
}

void SinkFcConfig::SetAuthMode(const AuthMode& authMode)
{
    mAuthMode = authMode;
}

AuthMode SinkFcConfig::GetAuthMode() const
{
    return mAuthMode;
}

void SinkFcConfig::SetAccessId(const std::string& accessId)
{
    mAccessId = accessId;
}

std::string SinkFcConfig::GetAccessId() const
{
    return mAccessId;
}

void SinkFcConfig::SetAccessKey(const std::string& accessKey)
{
    mAccessKey = accessKey;
}

std::string SinkFcConfig::GetAccessKey() const
{
    return mAccessKey;
}


SinkOssConfig::SinkOssConfig() :
        mTimeRange(-1),
        mAuthMode(AuthMode::AK)
{
}

SinkOssConfig::~SinkOssConfig()
{
}

void SinkOssConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value endpoint(rapidjson::kStringType);
    endpoint.SetString(mEndpoint.c_str(), allocator);
    jsonValue.AddMember("Endpoint", endpoint, allocator);

    rapidjson::Value bucket(rapidjson::kStringType);
    bucket.SetString(mBucket.c_str(), allocator);
    jsonValue.AddMember("Bucket", bucket, allocator);

    rapidjson::Value prefix(rapidjson::kStringType);
    prefix.SetString(mPrefix.c_str(), allocator);
    jsonValue.AddMember("Prefix", prefix, allocator);

    rapidjson::Value timeFormat(rapidjson::kStringType);
    timeFormat.SetString(mTimeFormat.c_str(), allocator);
    jsonValue.AddMember("TimeFormat", timeFormat, allocator);

    rapidjson::Value timeRange(rapidjson::kNumberType);
    timeRange.SetInt(mTimeRange);
    jsonValue.AddMember("TimeRange", timeRange, allocator);

    rapidjson::Value authMode(rapidjson::kStringType);
    authMode.SetString(Utils::ToLowerCaseString(GetNameForAuthMode(mAuthMode)).c_str(), allocator);
    jsonValue.AddMember("AuthMode", authMode, allocator);

    if (mAuthMode == AuthMode::AK)
    {
        rapidjson::Value accessId(rapidjson::kStringType);
        accessId.SetString(mAccessId.c_str(), allocator);
        jsonValue.AddMember("AccessId", accessId, allocator);

        rapidjson::Value accessKey(rapidjson::kStringType);
        accessKey.SetString(mAccessKey.c_str(), allocator);
        jsonValue.AddMember("AccessKey", accessKey, allocator);
    }
}

void SinkOssConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator endpointItr = jsonValue.FindMember("Endpoint");
    if (endpointItr != jsonValue.MemberEnd() && endpointItr->value.IsString())
    {
        mEndpoint.assign(endpointItr->value.GetString(), endpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator bucketItr = jsonValue.FindMember("Bucket");
    if (bucketItr != jsonValue.MemberEnd() && bucketItr->value.IsString())
    {
        mBucket.assign(bucketItr->value.GetString(), bucketItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator prefixItr = jsonValue.FindMember("Prefix");
    if (prefixItr != jsonValue.MemberEnd() && prefixItr->value.IsString())
    {
        mPrefix.assign(prefixItr->value.GetString(), prefixItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator timeFormatItr = jsonValue.FindMember("TimeFormat");
    if (timeFormatItr != jsonValue.MemberEnd() && timeFormatItr->value.IsString())
    {
        mTimeFormat.assign(timeFormatItr->value.GetString(), timeFormatItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator timeRangeItr = jsonValue.FindMember("TimeRange");
    if (timeRangeItr != jsonValue.MemberEnd() && timeRangeItr->value.IsString())
    {
        std::string timeRange = std::string(timeRangeItr->value.GetString(), timeRangeItr->value.GetStringLength());
        mTimeRange = std::stoi(timeRange);
    }

    rapidjson::Value::ConstMemberIterator authModeItr = jsonValue.FindMember("AuthMode");
    if (authModeItr != jsonValue.MemberEnd() && authModeItr->value.IsString())
    {
        std::string str = std::string(authModeItr->value.GetString(), authModeItr->value.GetStringLength());
        if (!str.empty())
        {
            mAuthMode = GetAuthModeFromName(Utils::ToUpperCaseString(str));
        }
    }

    rapidjson::Value::ConstMemberIterator accessIdItr = jsonValue.FindMember("AccessId");
    if (accessIdItr != jsonValue.MemberEnd() && accessIdItr->value.IsString())
    {
        mAccessId.assign(accessIdItr->value.GetString(), accessIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator accessKeyItr = jsonValue.FindMember("AccessKey");
    if (accessKeyItr != jsonValue.MemberEnd() && accessKeyItr->value.IsString())
    {
        mAccessKey.assign(accessKeyItr->value.GetString(), accessKeyItr->value.GetStringLength());
    }
}

void SinkOssConfig::SetEndpoint(const std::string& endpoint)
{
    mEndpoint = endpoint;
}

std::string SinkOssConfig::GetEndpoint() const
{
    return mEndpoint;
}

void SinkOssConfig::SetBucket(const std::string& bucket)
{
    mBucket = bucket;
}

std::string SinkOssConfig::GetBucket() const
{
    return mBucket;
}

void SinkOssConfig::SetPrefix(const std::string& prefix)
{
    mPrefix = prefix;
}

std::string SinkOssConfig::GetPrefix() const
{
    return mPrefix;
}

void SinkOssConfig::SetTimeFormat(const std::string& timeFormat)
{
    mTimeFormat = timeFormat;
}

std::string SinkOssConfig::GetTimeFormat() const
{
    return mTimeFormat;
}

void SinkOssConfig::SetTimeRange(int timeRange)
{
    mTimeRange = timeRange;
}

int SinkOssConfig::GetTimeRange() const
{
    return mTimeRange;
}

void SinkOssConfig::SetAuthMode(const AuthMode& authMode)
{
    mAuthMode = authMode;
}

AuthMode SinkOssConfig::GetAuthMode() const
{
    return mAuthMode;
}

void SinkOssConfig::SetAccessId(const std::string& accessId)
{
    mAccessId = accessId;
}

std::string SinkOssConfig::GetAccessId() const
{
    return mAccessId;
}

void SinkOssConfig::SetAccessKey(const std::string& accessKey)
{
    mAccessKey = accessKey;
}

std::string SinkOssConfig::GetAccessKey() const
{
    return mAccessKey;
}


SinkOtsConfig::SinkOtsConfig() :
        mAuthMode(AuthMode::AK)
{
}

SinkOtsConfig::~SinkOtsConfig()
{
}

void SinkOtsConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value endpoint(rapidjson::kStringType);
    endpoint.SetString(mEndpoint.c_str(), allocator);
    jsonValue.AddMember("Endpoint", endpoint, allocator);

    rapidjson::Value instance(rapidjson::kStringType);
    instance.SetString(mInstance.c_str(), allocator);
    jsonValue.AddMember("InstanceName", instance, allocator);

    rapidjson::Value table(rapidjson::kStringType);
    table.SetString(mTable.c_str(), allocator);
    jsonValue.AddMember("TableName", table, allocator);

    rapidjson::Value authMode(rapidjson::kStringType);
    authMode.SetString(Utils::ToLowerCaseString(GetNameForAuthMode(mAuthMode)).c_str(), allocator);
    jsonValue.AddMember("AuthMode", authMode, allocator);

    if (mAuthMode == AuthMode::AK)
    {
        rapidjson::Value accessId(rapidjson::kStringType);
        accessId.SetString(mAccessId.c_str(), allocator);
        jsonValue.AddMember("AccessId", accessId, allocator);

        rapidjson::Value accessKey(rapidjson::kStringType);
        accessKey.SetString(mAccessKey.c_str(), allocator);
        jsonValue.AddMember("AccessKey", accessKey, allocator);
    }
}

void SinkOtsConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator endpointItr = jsonValue.FindMember("Endpoint");
    if (endpointItr != jsonValue.MemberEnd() && endpointItr->value.IsString())
    {
        mEndpoint.assign(endpointItr->value.GetString(), endpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator instanceItr = jsonValue.FindMember("InstanceName");
    if (instanceItr != jsonValue.MemberEnd() && instanceItr->value.IsString())
    {
        mInstance.assign(instanceItr->value.GetString(), instanceItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator tableItr = jsonValue.FindMember("TableName");
    if (tableItr != jsonValue.MemberEnd() && tableItr->value.IsString())
    {
        mTable.assign(tableItr->value.GetString(), tableItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator authModeItr = jsonValue.FindMember("AuthMode");
    if (authModeItr != jsonValue.MemberEnd() && authModeItr->value.IsString())
    {
        std::string str = std::string(authModeItr->value.GetString(), authModeItr->value.GetStringLength());
        if (!str.empty())
        {
            mAuthMode = GetAuthModeFromName(Utils::ToUpperCaseString(str));
        }
    }

    rapidjson::Value::ConstMemberIterator accessIdItr = jsonValue.FindMember("AccessId");
    if (accessIdItr != jsonValue.MemberEnd() && accessIdItr->value.IsString())
    {
        mAccessId.assign(accessIdItr->value.GetString(), accessIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator accessKeyItr = jsonValue.FindMember("AccessKey");
    if (accessKeyItr != jsonValue.MemberEnd() && accessKeyItr->value.IsString())
    {
        mAccessKey.assign(accessKeyItr->value.GetString(), accessKeyItr->value.GetStringLength());
    }
}

void SinkOtsConfig::SetEndpoint(const std::string& endpoint)
{
    mEndpoint = endpoint;
}

std::string SinkOtsConfig::GetEndpoint() const
{
    return mEndpoint;
}

void SinkOtsConfig::SetInstance(const std::string& instance)
{
    mInstance = instance;
}

std::string SinkOtsConfig::GetInstance() const
{
    return mInstance;
}

void SinkOtsConfig::SetTable(const std::string& table)
{
    mTable = table;
}

std::string SinkOtsConfig::GetTable() const
{
    return mTable;
}

void SinkOtsConfig::SetAuthMode(const AuthMode& authMode)
{
    mAuthMode = authMode;
}

AuthMode SinkOtsConfig::GetAuthMode() const
{
    return mAuthMode;
}

void SinkOtsConfig::SetAccessId(const std::string& accessId)
{
    mAccessId = accessId;
}

std::string SinkOtsConfig::GetAccessId() const
{
    return mAccessId;
}

void SinkOtsConfig::SetAccessKey(const std::string& accessKey)
{
    mAccessKey = accessKey;
}

std::string SinkOtsConfig::GetAccessKey() const
{
    return mAccessKey;
}


std::string GetNameForInsertMode(const InsertMode& mode)
{
    switch (mode)
    {
        case IGNORE:
            return "IGNORE";
        case OVERWRITE:
            return "OVERWRITE";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid insert mode");
    }
}

InsertMode GetInsertModeFromName(const std::string& name)
{
    if (name == "IGNORE")
    {
        return IGNORE;
    }
    else if (name == "OVERWRITE")
    {
        return OVERWRITE;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported insert mode " + name);
    }
}

SinkMysqlConfig::SinkMysqlConfig() :
        mPort(0),
        mInsertMode(InsertMode::IGNORE)
{
}

SinkMysqlConfig::~SinkMysqlConfig()
{
}

void SinkMysqlConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value host(rapidjson::kStringType);
    host.SetString(mHost.c_str(), allocator);
    jsonValue.AddMember("Host", host, allocator);

    rapidjson::Value port(rapidjson::kStringType);
    port.SetString(std::to_string(mPort).c_str(), allocator);
    jsonValue.AddMember("Port", port, allocator);

    rapidjson::Value database(rapidjson::kStringType);
    database.SetString(mDatabase.c_str(), allocator);
    jsonValue.AddMember("Database", database, allocator);

    rapidjson::Value table(rapidjson::kStringType);
    table.SetString(mTable.c_str(), allocator);
    jsonValue.AddMember("Table", table, allocator);

    rapidjson::Value user(rapidjson::kStringType);
    user.SetString(mUser.c_str(), allocator);
    jsonValue.AddMember("User", user, allocator);

    rapidjson::Value password(rapidjson::kStringType);
    password.SetString(mPassword.c_str(), allocator);
    jsonValue.AddMember("Password", password, allocator);

//    rapidjson::Value insertMode(rapidjson::kStringType);
//    insertMode.SetString(Utils::ToLowerCaseString(GetNameForInsertMode(mInsertMode)).c_str(), allocator);
//    jsonValue.AddMember("InsertMode", insertMode, allocator);

    rapidjson::Value ignore(rapidjson::kStringType);
    bool bIgnore = (mInsertMode == InsertMode::IGNORE);
    ignore.SetString(Utils::BoolToString(bIgnore).c_str(), allocator);
    jsonValue.AddMember("Ignore", ignore, allocator);
}

void SinkMysqlConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator hostItr = jsonValue.FindMember("Host");
    if (hostItr != jsonValue.MemberEnd() && hostItr->value.IsString())
    {
        mHost.assign(hostItr->value.GetString(), hostItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator portItr = jsonValue.FindMember("Port");
    if (portItr != jsonValue.MemberEnd() && portItr->value.IsString())
    {
        std::string timeRange = std::string(portItr->value.GetString(), portItr->value.GetStringLength());
        mPort = std::stoi(timeRange);
    }

    rapidjson::Value::ConstMemberIterator databaseItr = jsonValue.FindMember("Database");
    if (databaseItr != jsonValue.MemberEnd() && databaseItr->value.IsString())
    {
        mDatabase.assign(databaseItr->value.GetString(), databaseItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator tableItr = jsonValue.FindMember("Table");
    if (tableItr != jsonValue.MemberEnd() && tableItr->value.IsString())
    {
        mTable.assign(tableItr->value.GetString(), tableItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator userItr = jsonValue.FindMember("User");
    if (userItr != jsonValue.MemberEnd() && userItr->value.IsString())
    {
        mUser.assign(userItr->value.GetString(), userItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator passwordItr = jsonValue.FindMember("Password");
    if (passwordItr != jsonValue.MemberEnd() && passwordItr->value.IsString())
    {
        mPassword.assign(passwordItr->value.GetString(), passwordItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator ignoreItr = jsonValue.FindMember("Ignore");
    if (ignoreItr != jsonValue.MemberEnd() && ignoreItr->value.IsString())
    {
        bool ignore = Utils::StringToBool(ignoreItr->value.GetString());
        mInsertMode = ignore ? InsertMode::IGNORE : InsertMode::OVERWRITE;
    }
}

void SinkMysqlConfig::SetHost(const std::string& host)
{
    mHost = host;
}

std::string SinkMysqlConfig::GetHost() const
{
    return mHost;
}

void SinkMysqlConfig::SetPort(int port)
{
    mPort = port;
}

int SinkMysqlConfig::GetPort() const
{
    return mPort;
}

void SinkMysqlConfig::SetDatabase(const std::string& database)
{
    mDatabase = database;
}

std::string SinkMysqlConfig::GetDatabase() const
{
    return mDatabase;
}

void SinkMysqlConfig::SetTable(const std::string& table)
{
    mTable = table;
}

std::string SinkMysqlConfig::GetTable() const
{
    return mTable;
}

void SinkMysqlConfig::SetUser(const std::string& user)
{
    mUser = user;
}

std::string SinkMysqlConfig::GetUser() const
{
    return mUser;
}

void SinkMysqlConfig::SetPassword(const std::string& password)
{
    mPassword = password;
}

std::string SinkMysqlConfig::GetPassword() const
{
    return mPassword;
}

void SinkMysqlConfig::SetInsertMode(const InsertMode& insertMode)
{
    mInsertMode = insertMode;
}

InsertMode SinkMysqlConfig::GetInsertMode() const
{
    return mInsertMode;
}


SinkAdsConfig::SinkAdsConfig() :
        SinkMysqlConfig()
{
}

SinkAdsConfig::~SinkAdsConfig()
{
}


std::string GetNameForPartitionMode(const PartitionMode& mode)
{
    switch (mode)
    {
        case USER_DEFINE:
            return "USER_DEFINE";
        case SYSTEM_TIME:
            return "SYSTEM_TIME";
        case EVENT_TIME:
            return "EVENT_TIME";
        case META_TIME:
            return "META_TIME";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid partition mode");
    }
}

PartitionMode GetPartitionModeFromName(const std::string& name)
{
    if (name == "USER_DEFINE")
    {
        return USER_DEFINE;
    }
    else if (name == "SYSTEM_TIME")
    {
        return SYSTEM_TIME;
    }
    else if (name == "EVENT_TIME")
    {
        return EVENT_TIME;
    }
    else if (name == "META_TIME")
    {
        return META_TIME;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported partition mode " + name);
    }
}

SinkOdpsConfig::SinkOdpsConfig() :
        mPartitionMode(PartitionMode::USER_DEFINE),
        mTimeRange(0),
        mBase64Encode(false)
{
}

SinkOdpsConfig::~SinkOdpsConfig()
{
}

void SinkOdpsConfig::SinkConfigToJson(rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue) const
{
    rapidjson::Value endpoint(rapidjson::kStringType);
    endpoint.SetString(mEndpoint.c_str(), allocator);
    jsonValue.AddMember("OdpsEndpoint", endpoint, allocator);

    rapidjson::Value project(rapidjson::kStringType);
    project.SetString(mProject.c_str(), allocator);
    jsonValue.AddMember("Project", project, allocator);

    rapidjson::Value table(rapidjson::kStringType);
    table.SetString(mTable.c_str(), allocator);
    jsonValue.AddMember("Table", table, allocator);

    if (!mTunnelEndpoint.empty())
    {
        rapidjson::Value tunnelEndpoint(rapidjson::kStringType);
        tunnelEndpoint.SetString(mTunnelEndpoint.c_str(), allocator);
        jsonValue.AddMember("TunnelEndpoint", tunnelEndpoint, allocator);
    }

    rapidjson::Value accessId(rapidjson::kStringType);
    accessId.SetString(mAccessId.c_str(), allocator);
    jsonValue.AddMember("AccessId", accessId, allocator);

    rapidjson::Value accessKey(rapidjson::kStringType);
    accessKey.SetString(mAccessKey.c_str(), allocator);
    jsonValue.AddMember("AccessKey", accessKey, allocator);

    rapidjson::Value partitionMode(rapidjson::kStringType);
    partitionMode.SetString(GetNameForPartitionMode(mPartitionMode).c_str(), allocator);
    jsonValue.AddMember("PartitionMode", partitionMode, allocator);

    if (mPartitionMode != PartitionMode::USER_DEFINE)
    {
        rapidjson::Value timeRange(rapidjson::kNumberType);
        timeRange.SetInt(mTimeRange);
        jsonValue.AddMember("TimeRange", timeRange, allocator);

        if (!mTimeZone.empty())
        {
            rapidjson::Value timeZone(rapidjson::kStringType);
            timeZone.SetString(mTimeZone.c_str(), allocator);
            jsonValue.AddMember("TimeZone", timeZone, allocator);
        }
    }

    rapidjson::Value partitionConfig(rapidjson::kObjectType);
    std::vector<std::pair<std::string, std::string> >::const_iterator pcIter;
    for (pcIter = mPartitionConfig.begin(); pcIter != mPartitionConfig.end(); ++pcIter)
    {
        rapidjson::Value key(rapidjson::kStringType);
        key.SetString(pcIter->first.c_str(), allocator);

        rapidjson::Value value(rapidjson::kStringType);
        value.SetString(pcIter->second.c_str(), allocator);

        partitionConfig.AddMember(key, value , allocator);
    }
    jsonValue.AddMember("PartitionConfig", partitionConfig, allocator);

    if (!mSplitKey.empty())
    {
        rapidjson::Value splitKey(rapidjson::kStringType);
        splitKey.SetString(mSplitKey.c_str(), allocator);
        jsonValue.AddMember("SplitKey", splitKey, allocator);
    }

    rapidjson::Value base64Encode(rapidjson::kStringType);
    base64Encode.SetString(Utils::BoolToString(mBase64Encode).c_str(), allocator);
    jsonValue.AddMember("Base64Encode", base64Encode, allocator);
}

void SinkOdpsConfig::JsonToSinkConfig(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator endpointItr = jsonValue.FindMember("OdpsEndpoint");
    if (endpointItr != jsonValue.MemberEnd() && endpointItr->value.IsString())
    {
        mEndpoint.assign(endpointItr->value.GetString(), endpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator projectItr = jsonValue.FindMember("Project");
    if (projectItr != jsonValue.MemberEnd() && projectItr->value.IsString())
    {
        mProject.assign(projectItr->value.GetString(), projectItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator tableItr = jsonValue.FindMember("Table");
    if (tableItr != jsonValue.MemberEnd() && tableItr->value.IsString())
    {
        mTable.assign(tableItr->value.GetString(), tableItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator tunnelEndpointItr = jsonValue.FindMember("TunnelEndpoint");
    if (tunnelEndpointItr != jsonValue.MemberEnd() && tunnelEndpointItr->value.IsString())
    {
        mTunnelEndpoint.assign(tunnelEndpointItr->value.GetString(), tunnelEndpointItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator accessIdItr = jsonValue.FindMember("AccessId");
    if (accessIdItr != jsonValue.MemberEnd() && accessIdItr->value.IsString())
    {
        mAccessId.assign(accessIdItr->value.GetString(), accessIdItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator accessKeyItr = jsonValue.FindMember("AccessKey");
    if (accessKeyItr != jsonValue.MemberEnd() && accessKeyItr->value.IsString())
    {
        mAccessKey.assign(accessKeyItr->value.GetString(), accessKeyItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator partitionModeItr = jsonValue.FindMember("PartitionMode");
    if (partitionModeItr != jsonValue.MemberEnd() && partitionModeItr->value.IsString())
    {
        std::string str = std::string(partitionModeItr->value.GetString(), partitionModeItr->value.GetStringLength());
        if (!str.empty())
        {
            mPartitionMode = GetPartitionModeFromName(Utils::ToUpperCaseString(str));
        }
    }

    rapidjson::Value::ConstMemberIterator timeRangeItr = jsonValue.FindMember("TimeRange");
    if (timeRangeItr != jsonValue.MemberEnd() && timeRangeItr->value.IsString())
    {
        std::string timeRange = std::string(timeRangeItr->value.GetString(), timeRangeItr->value.GetStringLength());
        mTimeRange = std::stoi(timeRange);
    }

    rapidjson::Value::ConstMemberIterator timeZoneItr = jsonValue.FindMember("TimeZone");
    if (timeZoneItr != jsonValue.MemberEnd() && timeZoneItr->value.IsString())
    {
        mTimeZone.assign(timeZoneItr->value.GetString(), timeZoneItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator partitionConfigItr = jsonValue.FindMember("PartitionConfig");
    if (partitionConfigItr != jsonValue.MemberEnd() && partitionConfigItr->value.IsString())
    {
        std::string partitionConfig = std::string(partitionConfigItr->value.GetString(), partitionConfigItr->value.GetStringLength());

        rapidjson::Document doc;
        doc.ParseInsitu(const_cast<char*>(partitionConfig.c_str()));

        if (doc.IsArray())
        {
            for (rapidjson::Value::ConstValueIterator itr = doc.Begin(); itr != doc.End(); ++itr)
            {
                const rapidjson::Value& config = *itr;
                if (config.IsObject())
                {
                    std::string key;
                    rapidjson::Value::ConstMemberIterator keyItr = config.FindMember("key");
                    if (keyItr != jsonValue.MemberEnd() && keyItr->value.IsString())
                    {
                        key.assign(keyItr->value.GetString(), keyItr->value.GetStringLength());
                    }

                    std::string value;
                    rapidjson::Value::ConstMemberIterator valueItr = config.FindMember("value");
                    if (valueItr != jsonValue.MemberEnd() && valueItr->value.IsString())
                    {
                        value.assign(valueItr->value.GetString(), valueItr->value.GetStringLength());
                    }

                    if (!key.empty())
                    {
                        mPartitionConfig.push_back(std::pair<std::string, std::string>(key, value));
                    }
                }
            }
        }
    }

    rapidjson::Value::ConstMemberIterator splitKeyItr = jsonValue.FindMember("SplitKey");
    if (splitKeyItr != jsonValue.MemberEnd() && splitKeyItr->value.IsString())
    {
        mSplitKey.assign(splitKeyItr->value.GetString(), splitKeyItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator base64EncodeItr = jsonValue.FindMember("Base64Encode");
    if (base64EncodeItr != jsonValue.MemberEnd() && base64EncodeItr->value.IsString())
    {
        mBase64Encode = Utils::StringToBool(base64EncodeItr->value.GetString());
    }
}

void SinkOdpsConfig::SetEndpoint(const std::string& endpoint)
{
    mEndpoint = endpoint;
}

std::string SinkOdpsConfig::GetEndpoint() const
{
    return mEndpoint;
}

void SinkOdpsConfig::SetProject(const std::string& project)
{
    mProject = project;
}

std::string SinkOdpsConfig::GetProject() const
{
    return mProject;
}

void SinkOdpsConfig::SetTable(const std::string& table)
{
    mTable = table;
}

std::string SinkOdpsConfig::GetTable() const
{
    return mTable;
}

void SinkOdpsConfig::SetTunnelEndpoint(const std::string& tunnelEndpoint)
{
    mTunnelEndpoint = tunnelEndpoint;
}

std::string SinkOdpsConfig::GetTunnelEndpoint() const
{
    return mTunnelEndpoint;
}

void SinkOdpsConfig::SetAccessId(const std::string& accessId)
{
    mAccessId = accessId;
}

std::string SinkOdpsConfig::GetAccessId() const
{
    return mAccessId;
}

void SinkOdpsConfig::SetAccessKey(const std::string& accessKey)
{
    mAccessKey = accessKey;
}

std::string SinkOdpsConfig::GetAccessKey() const
{
    return mAccessKey;
}

void SinkOdpsConfig::SetPartitionMode(const PartitionMode& partitionMode)
{
    mPartitionMode = partitionMode;
}

PartitionMode SinkOdpsConfig::GetPartitionMode() const
{
    return mPartitionMode;
}

void SinkOdpsConfig::SetTimeRange(int timeRange)
{
    mTimeRange = timeRange;
}

int SinkOdpsConfig::GetTimeRange() const
{
    return mTimeRange;
}

void SinkOdpsConfig::SetTimeZone(const std::string& timeZone)
{
    mTimeZone = timeZone;
}

std::string SinkOdpsConfig::GetTimeZone() const
{
    return mTimeZone;
}

void SinkOdpsConfig::SetPartitionConfig(const StringPairVec& partitionConfig)
{
    mPartitionConfig = partitionConfig;
}

const StringPairVec& SinkOdpsConfig::GetPartitionConfig() const
{
    return mPartitionConfig;
}

void SinkOdpsConfig::SetSplitKey(const std::string& splitKey)
{
    mSplitKey = splitKey;
}

std::string SinkOdpsConfig::GetSplitKey() const
{
    return mSplitKey;
}

void SinkOdpsConfig::SetBase64Encode(bool base64Encode)
{
    mBase64Encode = base64Encode;
}

bool SinkOdpsConfig::GetBase64Encode() const
{
    return mBase64Encode;
}


std::string GetNameForConnectorShardState(const ConnectorShardState &state)
{
    switch (state)
    {
        case CONTEXT_HANG:
            return "CONTEXT_HANG";
        case CONTEXT_PLANNED:
            return "CONTEXT_PLANNED";
        case CONTEXT_EXECUTING:
            return "CONTEXT_EXECUTING";
        case CONTEXT_PAUSED:
            return "CONTEXT_PAUSED";
        case CONTEXT_STOPPED:
            return "CONTEXT_STOPPED";
        case CONTEXT_FINISHED:
            return "CONTEXT_FINISHED";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid connector shard state");
    }
}

ConnectorShardState GetConnectorShardStateFromName(const std::string& name)
{
    if (name == "CONTEXT_HANG")
    {
        return CONTEXT_HANG;
    }
    else if (name == "CONTEXT_PLANNED")
    {
        return CONTEXT_PLANNED;
    }
    else if (name == "CONTEXT_EXECUTING")
    {
        return CONTEXT_EXECUTING;
    }
    else if (name == "CONTEXT_PAUSED")
    {
        return CONTEXT_PAUSED;
    }
    else if (name == "CONTEXT_STOPPED")
    {
        return CONTEXT_STOPPED;
    }
    else if (name == "CONTEXT_FINISHED")
    {
        return CONTEXT_FINISHED;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported connector shard state " + name);
    }
}

ConnectorShardStatusEntry::ConnectorShardStatusEntry() :
    mStartSequence(0),
    mEndSequence(0),
    mCurrentSequence(0),
    mCurrentTimestamp(0),
    mUpdateTime(0),
    mState(ConnectorShardState::CONTEXT_HANG),
    mDiscardCount(0),
    mDoneTime(0)
{
}

ConnectorShardStatusEntry::~ConnectorShardStatusEntry()
{
}

int64_t ConnectorShardStatusEntry::GetStartSequence() const
{
    return mStartSequence;
}

int64_t ConnectorShardStatusEntry::GetEndSequence() const
{
    return mEndSequence;
}

int64_t ConnectorShardStatusEntry::GetCurrentSequence() const
{
    return mCurrentSequence;
}

int64_t ConnectorShardStatusEntry::GetCurrentTimestamp() const
{
    return mCurrentTimestamp;
}

int64_t ConnectorShardStatusEntry::GetUpdateTime() const
{
    return mUpdateTime;
}

ConnectorShardState ConnectorShardStatusEntry::GetState() const
{
    return mState;
}

std::string ConnectorShardStatusEntry::GetLastErrorMessage() const
{
    return mLastErrorMessage;
}

int64_t ConnectorShardStatusEntry::GetDiscardCount() const
{
    return mDiscardCount;
}

int64_t ConnectorShardStatusEntry::GetDoneTime() const
{
    return mDoneTime;
}

std::string ConnectorShardStatusEntry::GetWorkerAddress() const
{
    return mWorkerAddress;
}

void ConnectorShardStatusEntry::JsonToShardStatusEntry(const rapidjson::Value& jsonValue)
{
    rapidjson::Value::ConstMemberIterator ssItr = jsonValue.FindMember("StartSequence");
    if (ssItr != jsonValue.MemberEnd() && ssItr->value.IsInt64())
    {
        mStartSequence = ssItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator esItr = jsonValue.FindMember("EndSequence");
    if (esItr != jsonValue.MemberEnd() && esItr->value.IsInt64())
    {
        mEndSequence = esItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator csItr = jsonValue.FindMember("CurrentSequence");
    if (csItr != jsonValue.MemberEnd() && csItr->value.IsInt64())
    {
        mCurrentSequence = csItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator ctItr = jsonValue.FindMember("CurrentTimestamp");
    if (ctItr != jsonValue.MemberEnd() && ctItr->value.IsInt64())
    {
        mCurrentTimestamp = ctItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator utItr = jsonValue.FindMember("UpdateTime");
    if (utItr != jsonValue.MemberEnd() && utItr->value.IsInt64())
    {
        mUpdateTime = utItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator stateItr = jsonValue.FindMember("State");
    if (stateItr != jsonValue.MemberEnd() && stateItr->value.IsString())
    {
        std::string stateStr = std::string(stateItr->value.GetString(), stateItr->value.GetStringLength());
        if (!stateStr.empty())
        {
            mState = GetConnectorShardStateFromName(Utils::ToUpperCaseString(stateStr));
        }
    }

    rapidjson::Value::ConstMemberIterator lemItr = jsonValue.FindMember("LastErrorMessage");
    if (lemItr != jsonValue.MemberEnd() && lemItr->value.IsString())
    {
        mLastErrorMessage.assign(lemItr->value.GetString(), lemItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator dcItr = jsonValue.FindMember("DiscardCount");
    if (dcItr != jsonValue.MemberEnd() && dcItr->value.IsInt64())
    {
        mDiscardCount = dcItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator dtItr = jsonValue.FindMember("DoneTime");
    if (dtItr != jsonValue.MemberEnd() && dtItr->value.IsInt64())
    {
        mDoneTime = dtItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator waItr = jsonValue.FindMember("WorkerAddress");
    if (waItr != jsonValue.MemberEnd() && waItr->value.IsString())
    {
        mWorkerAddress.assign(waItr->value.GetString(), waItr->value.GetStringLength());
    }
}


ConnectorOffset::ConnectorOffset() :
        mTimestamp(-1),
        mSequence(-1)
{
}

ConnectorOffset::ConnectorOffset(
        int64_t timestamp,
        int64_t sequence) :
        mTimestamp(timestamp),
        mSequence(sequence)
{
}

ConnectorOffset::~ConnectorOffset()
{
}

int64_t ConnectorOffset::GetTimestamp() const
{
    return mTimestamp;
}

int64_t ConnectorOffset::GetSequence() const
{
    return mSequence;
}

} // namespace sdk
} // namespace datahub
} // namespace aliyun
