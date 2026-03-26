#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "utils.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetConnectorResult::GetConnectorResult() :
        mType(sdk::ConnectorType::SINK_ODPS),
        mState(sdk::ConnectorState::CONNECTOR_CREATED),
        mCreateTime(0),
        mLastModifyTime(0)
{
}

GetConnectorResult::~GetConnectorResult()
{
}

void GetConnectorResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator caItr = doc.FindMember("ClusterAddress");
    if (caItr != doc.MemberEnd() && caItr->value.IsString())
    {
        mClusterAddr.assign(caItr->value.GetString(), caItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator ciItr = doc.FindMember("ConnectorId");
    if (ciItr != doc.MemberEnd() && ciItr->value.IsString())
    {
        mConnectorId.assign(ciItr->value.GetString(), ciItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator typeItr = doc.FindMember("Type");
    if (typeItr != doc.MemberEnd() && typeItr->value.IsString())
    {
        mType = sdk::GetConnectorTypeFromName(Utils::ToUpperCaseString(typeItr->value.GetString()));
    }

    rapidjson::Value::ConstMemberIterator stateItr = doc.FindMember("State");
    if (stateItr != doc.MemberEnd() && stateItr->value.IsString())
    {
        mState = sdk::GetConnectorStateFromName(Utils::ToUpperCaseString(stateItr->value.GetString()));
    }

    rapidjson::Value::ConstMemberIterator cfItr = doc.FindMember("ColumnFields");
    if (cfItr != doc.MemberEnd() && cfItr->value.IsArray())
    {
        const rapidjson::Value& columnFields = cfItr->value;
        for (rapidjson::SizeType i = 0; i < columnFields.Size(); ++i)
        {
            std::string columnField = columnFields[i].GetString();
            mColumnFields.push_back(columnField);
        }
    }

    rapidjson::Value::ConstMemberIterator creatorItr = doc.FindMember("Creator");
    if (creatorItr != doc.MemberEnd() && creatorItr->value.IsString())
    {
        mCreator.assign(creatorItr->value.GetString(), creatorItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator createTimeItr = doc.FindMember("CreateTime");
    if (createTimeItr != doc.MemberEnd() && createTimeItr->value.IsInt())
    {
        mCreateTime = createTimeItr->value.GetInt() * 1000l;
    }

    rapidjson::Value::ConstMemberIterator lastModifyTimeItr = doc.FindMember("LastModifyTime");
    if (lastModifyTimeItr != doc.MemberEnd() && lastModifyTimeItr->value.IsInt())
    {
        mLastModifyTime = lastModifyTimeItr->value.GetInt() * 1000l;
    }

    rapidjson::Value::ConstMemberIterator configItr = doc.FindMember("Config");
    if (configItr != doc.MemberEnd() && configItr->value.IsObject())
    {
        if (mType == sdk::ConnectorType::SINK_ODPS)
        {
            mConfig.reset(new sdk::SinkOdpsConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_DATAHUB)
        {
            mConfig.reset(new sdk::SinkDatahubConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_OSS)
        {
            mConfig.reset(new sdk::SinkOssConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_ES)
        {
            mConfig.reset(new sdk::SinkEsConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_ADS)
        {
            mConfig.reset(new sdk::SinkAdsConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_MYSQL)
        {
            mConfig.reset(new sdk::SinkMysqlConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_FC)
        {
            mConfig.reset(new sdk::SinkFcConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
        else if (mType == sdk::ConnectorType::SINK_OTS)
        {
            mConfig.reset(new sdk::SinkOtsConfig());
            mConfig->JsonToSinkConfig(configItr->value);
        }
    }

    rapidjson::Value::ConstMemberIterator extraConfigItr = doc.FindMember("ExtraInfo");
    if (extraConfigItr != doc.MemberEnd() && extraConfigItr->value.IsObject())
    {
        const rapidjson::Value& extraConfig = extraConfigItr->value;
        for (rapidjson::Value::ConstMemberIterator offsetItr = extraConfig.MemberBegin();
             offsetItr != extraConfig.MemberEnd(); ++offsetItr)
        {
            const std::string key = offsetItr->name.GetString();
            const rapidjson::Value& value = offsetItr->value;
            mExtraConfig.insert(std::pair<std::string, std::string>(key, value.GetString()));

            if (key == "SubscriptionId")
            {
                mSubId.assign(value.GetString(), value.GetStringLength());
            }
        }
    }
}

std::string GetConnectorResult::GetClusterAddr() const
{
    return mClusterAddr;
}

std::string GetConnectorResult::GetConnectorId() const
{
    return mConnectorId;
}

sdk::ConnectorType GetConnectorResult::GetType() const
{
    return mType;
}

sdk::ConnectorState GetConnectorResult::GetState() const
{
    return mState;
}

StringVec GetConnectorResult::GetColumnFields() const
{
    return mColumnFields;
}

std::string GetConnectorResult::GetCreator() const
{
    return mCreator;
}

int64_t GetConnectorResult::GetCreateTime() const
{
    return mCreateTime;
}

int64_t GetConnectorResult::GetLastModifyTime() const
{
    return mLastModifyTime;
}

const sdk::SinkConfig* GetConnectorResult::GetConfig() const
{
    return mConfig.get();
}

std::map<std::string, std::string> GetConnectorResult::GetExtraConfig() const
{
    return mExtraConfig;
}

std::string GetConnectorResult::GetSubId() const
{
    return mSubId;
}

} // namespace datahub
} // namespace aliyun
