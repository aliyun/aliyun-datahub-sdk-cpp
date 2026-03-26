#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

CreateSubscriptionResult::CreateSubscriptionResult()
{
}

CreateSubscriptionResult::~CreateSubscriptionResult()
{
}

void CreateSubscriptionResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator subIdItr1 = doc.FindMember("SubId");
    if (subIdItr1 != doc.MemberEnd() && subIdItr1->value.IsString())
    {
        mSubId.assign(subIdItr1->value.GetString(), subIdItr1->value.GetStringLength());
    }
}

std::string CreateSubscriptionResult::GetSubId() const
{
    return mSubId;
}

} // namespace datahub
} // namespace aliyun
