#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

CreateSubscriptionRequest::CreateSubscriptionRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& comment) :
        mProject(project),
        mTopic(topic),
        mAction("Create"),
        mComment(comment)
{
}

CreateSubscriptionRequest::~CreateSubscriptionRequest()
{
}

std::string CreateSubscriptionRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    writer.Key("Comment");
    writer.String(mComment.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

std::string CreateSubscriptionRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions");
    return path;
}

} // namespace datahub
} // namespace aliyun
