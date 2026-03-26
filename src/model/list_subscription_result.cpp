#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

ListSubscriptionResult::ListSubscriptionResult() :
        mTotalCount(0)
{
}

ListSubscriptionResult::~ListSubscriptionResult()
{
}

void ListSubscriptionResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator subscriptionsItr = doc.FindMember("Subscriptions");
    if (subscriptionsItr != doc.MemberEnd() && subscriptionsItr->value.IsArray())
    {
        const rapidjson::Value& subscriptionsJson = subscriptionsItr->value;
        for (rapidjson::SizeType i = 0; i < subscriptionsJson.Size(); ++i)
        {
            const rapidjson::Value& subscriptionJson = subscriptionsJson[i];
            if (subscriptionJson.IsObject())
            {
                SubscriptionEntry subscriptionEntry;
                JsonTool::JsonToSubscriptionEntry(subscriptionJson, subscriptionEntry);
                mSubscriptions.push_back(subscriptionEntry);
            }
        }
    }

    rapidjson::Value::ConstMemberIterator totalCountItr = doc.FindMember("TotalCount");
    if (totalCountItr != doc.MemberEnd() && totalCountItr->value.IsInt64())
    {
        mTotalCount = totalCountItr->value.GetInt64();
    }
}

const std::vector<SubscriptionEntry>& ListSubscriptionResult::GetSubscriptions() const
{
    return mSubscriptions;
}

int64_t ListSubscriptionResult::GetTotalCount() const
{
    return mTotalCount;
}

} // namespace datahub
} // namespace aliyun
