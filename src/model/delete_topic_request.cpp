#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

DeleteTopicRequest::DeleteTopicRequest(
        const std::string& project,
        const std::string& topic) :
        mProject(project),
        mTopic(topic)
{
}

DeleteTopicRequest::~DeleteTopicRequest()
{
}

std::string DeleteTopicRequest::SerializePayload() const
{
    std::string body;
    return body;
}

std::string DeleteTopicRequest::BuildPath() const
{
    std::string path("/projects/" + mProject + "/topics/" + mTopic);
    return path;
}

} // namespace datahub
} // namespace aliyun
