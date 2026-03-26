#include "datahub/datahub_exception.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetTopicSchemaResult::GetTopicSchemaResult() :
    mVersionId(-1)
{
}

GetTopicSchemaResult::~GetTopicSchemaResult()
{
}

int64_t GetTopicSchemaResult::GetVersionId() const
{
    return mVersionId;
}

int64_t GetTopicSchemaResult::GetCreateTime() const
{
    return mCreateTime;
}

std::string GetTopicSchemaResult::GetCreator() const
{
    return mCreator;
}

const RecordSchema& GetTopicSchemaResult::GetRecordSchema() const
{
    return mRecordSchema;
}

void GetTopicSchemaResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator valueItr = doc.FindMember("VersionId");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsInt64())
    {
        mVersionId = valueItr->value.GetInt64();
    }

    valueItr = doc.FindMember("CreateTime");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsInt64())
    {
        mCreateTime = valueItr->value.GetInt64();
    }

    valueItr = doc.FindMember("Creator");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsString())
    {
        mCreator = valueItr->value.GetString();
    }

    valueItr = doc.FindMember("RecordSchema");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsString())
    {
        mRecordSchema.FromJsonString(valueItr->value.GetString());
    }
}

} // namespace datahub
} // namespace aliyun