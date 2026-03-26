#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"

namespace aliyun
{
namespace datahub
{

GetSubscriptionResult::GetSubscriptionResult()
{
}

GetSubscriptionResult::~GetSubscriptionResult()
{
}

void GetSubscriptionResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    doc.ParseInsitu(const_cast<char *>(payload.c_str()));

    JsonTool::JsonToSubscriptionEntry(doc, subscriptionEntry);
}

std::string GetSubscriptionResult::GetSubId() const
{
    return subscriptionEntry.GetSubId();
}

std::string GetSubscriptionResult::GetComment() const
{
    return subscriptionEntry.GetComment();
}

bool GetSubscriptionResult::GetIsOwner() const
{
    return subscriptionEntry.GetIsOwner();
}

SubscriptionType GetSubscriptionResult::GetType() const
{
    return subscriptionEntry.GetType();
}

SubscriptionState GetSubscriptionResult::GetState() const
{
    return subscriptionEntry.GetState();
}

int64_t GetSubscriptionResult::GetCreateTime() const
{
    return subscriptionEntry.GetCreateTime();
}

int64_t GetSubscriptionResult::GetLastModifyTime() const
{
    return subscriptionEntry.GetLastModifyTime();
}

} // namespace datahub
} // namespace aliyun
