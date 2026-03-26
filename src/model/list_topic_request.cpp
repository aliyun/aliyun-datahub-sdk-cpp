#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

ListTopicRequest::ListTopicRequest(
        const std::string& project) :
        mProject(project)
{
}

ListTopicRequest::~ListTopicRequest()
{
}

std::string ListTopicRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics");
    return path;
}

std::string ListTopicRequest::SerializePayload() const
{
    std::string body;
    return body;
}

} // namespace datahub
} // namespace aliyun
