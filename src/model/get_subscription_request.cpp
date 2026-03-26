#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

GetSubscriptionRequest::GetSubscriptionRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& subId) :
        mProject(project),
        mTopic(topic),
        mSubId(subId)
{
}

GetSubscriptionRequest::~GetSubscriptionRequest()
{
}

std::string GetSubscriptionRequest::SerializePayload() const
{
    std::string body;
    return body;
}

std::string GetSubscriptionRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mSubId);
    return path;
}

} // namespace datahub
} // namespace aliyun
