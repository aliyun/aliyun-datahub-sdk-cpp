#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

DeleteSubscriptionRequest::DeleteSubscriptionRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& subId) :
        mProject(project),
        mTopic(topic),
        mSubId(subId)
{
}

DeleteSubscriptionRequest::~DeleteSubscriptionRequest()
{
}

std::string DeleteSubscriptionRequest::SerializePayload() const
{
    std::string body;
    return body;
}

std::string DeleteSubscriptionRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mSubId);
    return path;
}

} // namespace datahub
} // namespace aliyun
