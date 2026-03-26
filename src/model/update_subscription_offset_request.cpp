#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"
#include "json_tool.h"

namespace aliyun
{
namespace datahub
{

UpdateSubscriptionOffsetRequest::UpdateSubscriptionOffsetRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::map<std::string, SubscriptionOffset>& offsets) :
        mProject(project),
        mTopic(topic),
        mSubId(subId),
        mAction("commit"),
        mOffsets(offsets)
{
}

UpdateSubscriptionOffsetRequest::~UpdateSubscriptionOffsetRequest()
{
}

std::string UpdateSubscriptionOffsetRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mSubId).append("/offsets");
    return path;
}

std::string UpdateSubscriptionOffsetRequest::SerializePayload() const
{
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value offsetsJson(rapidjson::kObjectType);
    std::map<std::string, SubscriptionOffset>::const_iterator iter;
    for (iter = mOffsets.begin(); iter != mOffsets.end(); ++iter)
    {
        rapidjson::Value shardId(rapidjson::kStringType);
        shardId.SetString(iter->first.c_str(), allocator);

        rapidjson::Value offsetJsonValue(rapidjson::kObjectType);
        JsonTool::SubscriptionOffsetToJson(iter->second, allocator, offsetJsonValue);

        offsetsJson.AddMember(shardId, offsetJsonValue , allocator);
    }

    doc.AddMember("Offsets", offsetsJson, allocator);

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    doc.AddMember("Action", action, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
