#include "datahub/datahub_exception.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetConnectorDoneTimeResult::GetConnectorDoneTimeResult() :
        mDoneTime(0)
{
}

GetConnectorDoneTimeResult::~GetConnectorDoneTimeResult()
{
}

void GetConnectorDoneTimeResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator doneTimeItr = doc.FindMember("DoneTime");
    if (doneTimeItr != doc.MemberEnd() && doneTimeItr->value.IsInt64())
    {
        mDoneTime = doneTimeItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator timeZoneItr = doc.FindMember("TimeZone");
    if (timeZoneItr != doc.MemberEnd() && timeZoneItr->value.IsString())
    {
        mTimeZone.assign(timeZoneItr->value.GetString(), timeZoneItr->value.GetStringLength());
    }
}

int64_t GetConnectorDoneTimeResult::GetDoneTime() const
{
    return mDoneTime;
}

std::string GetConnectorDoneTimeResult::GetTimeZone() const
{
    return mTimeZone;
}

} // namespace datahub
} // namespace aliyun
