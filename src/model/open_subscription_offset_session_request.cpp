#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

OpenSubscriptionOffsetSessionRequest::OpenSubscriptionOffsetSessionRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const StringVec& shardIds) :
        mProject(project),
        mTopic(topic),
        mSubId(subId),
        mAction("open"),
        mShardIds(shardIds)
{
}

OpenSubscriptionOffsetSessionRequest::~OpenSubscriptionOffsetSessionRequest()
{
}

std::string OpenSubscriptionOffsetSessionRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mSubId).append("/offsets");
    return path;
}

std::string OpenSubscriptionOffsetSessionRequest::SerializePayload() const
{
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value shardIdsJson(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator iter;
    for (iter = mShardIds.begin(); iter != mShardIds.end(); ++iter)
    {
        rapidjson::Value str;
        if (!iter->empty())
        {
            str.SetString(iter->c_str(), iter->size(), allocator);
            shardIdsJson.PushBack(str, allocator);
        }
    }
    doc.AddMember("ShardIds", shardIdsJson, allocator);

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
