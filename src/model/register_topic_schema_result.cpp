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

RegisterTopicSchemaResult::RegisterTopicSchemaResult() :
    mVersionId(-1)
{
}

RegisterTopicSchemaResult::~RegisterTopicSchemaResult()
{
}

int64_t RegisterTopicSchemaResult::GetVersionId() const
{
    return mVersionId;
}

void RegisterTopicSchemaResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator versionIdItr = doc.FindMember("VersionId");
    if (versionIdItr != doc.MemberEnd() && versionIdItr->value.IsInt64())
    {
        mVersionId = versionIdItr->value.GetInt64();
    }
}

} // namespace datahub
} // namespace aliyun