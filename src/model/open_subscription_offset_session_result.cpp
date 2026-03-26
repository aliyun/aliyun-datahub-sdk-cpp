#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

OpenSubscriptionOffsetSessionResult::OpenSubscriptionOffsetSessionResult()
{
}

OpenSubscriptionOffsetSessionResult::~OpenSubscriptionOffsetSessionResult()
{
}

void OpenSubscriptionOffsetSessionResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator offsetsItr = doc.FindMember("Offsets");
    if (offsetsItr != doc.MemberEnd() && offsetsItr->value.IsObject())
    {
        const rapidjson::Value& offsetsJson = offsetsItr->value;
        for (rapidjson::Value::ConstMemberIterator offsetItr = offsetsJson.MemberBegin();
                offsetItr != offsetsJson.MemberEnd(); ++offsetItr)
        {
            const std::string shardId = offsetItr->name.GetString();
            const rapidjson::Value& offsetJson = offsetItr->value;
            if (offsetJson.IsObject())
            {
                SubscriptionOffset subscriptionOffset;
                JsonTool::JsonToSubscriptionOffset(offsetJson, subscriptionOffset);
                mOffsets.insert(std::pair<std::string, SubscriptionOffset>(shardId, subscriptionOffset));
            }
        }
    }
}

const std::map<std::string, SubscriptionOffset>& OpenSubscriptionOffsetSessionResult::GetOffsets() const
{
    return mOffsets;
}

} // namespace datahub
} // namespace aliyun
