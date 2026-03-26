#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

UpdateSubscriptionStateRequest::UpdateSubscriptionStateRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const SubscriptionState& state) :
        mProject(project),
        mTopic(topic),
        mSubId(subId),
        mState(state)
{
}

UpdateSubscriptionStateRequest::~UpdateSubscriptionStateRequest()
{
}

std::string UpdateSubscriptionStateRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mSubId);
    return path;
}

std::string UpdateSubscriptionStateRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("State");
    writer.Int(mState);
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
