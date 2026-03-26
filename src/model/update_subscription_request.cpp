#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

UpdateSubscriptionRequest::UpdateSubscriptionRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::string& comment) :
        mProject(project),
        mTopic(topic),
        mSubId(subId),
        mComment(comment)
{
}

UpdateSubscriptionRequest::~UpdateSubscriptionRequest()
{
}

std::string UpdateSubscriptionRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mSubId);
    return path;
}

std::string UpdateSubscriptionRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Comment");
    writer.String(mComment.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
