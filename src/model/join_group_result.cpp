#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

JoinGroupResult::JoinGroupResult() :
        mVersionId(-1),
        mSessionTimeout(0)
{
}

JoinGroupResult::~JoinGroupResult()
{
}

void JoinGroupResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator ciItr = doc.FindMember("ConsumerId");
    if (ciItr != doc.MemberEnd() && ciItr->value.IsString())
    {
        mConsumerId.assign(ciItr->value.GetString(), ciItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator viItr = doc.FindMember("VersionId");
    if (viItr != doc.MemberEnd() && viItr->value.IsInt64())
    {
        mVersionId = viItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator stItr = doc.FindMember("SessionTimeout");
    if (stItr != doc.MemberEnd() && stItr->value.IsInt64())
    {
        mSessionTimeout = stItr->value.GetInt64();
    }
}

std::string JoinGroupResult::GetConsumerId() const
{
    return mConsumerId;
}

int64_t JoinGroupResult::GetVersionId() const
{
    return mVersionId;
}

int64_t JoinGroupResult::GetSessionTimeout() const
{
    return mSessionTimeout;
}

} // namespace datahub
} // namespace aliyun
