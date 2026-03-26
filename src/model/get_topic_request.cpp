#include <string>
#include <memory>
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"

namespace aliyun
{
namespace datahub
{

GetTopicRequest::GetTopicRequest(
        const std::string& project,
        const std::string& topic) :
        mProject(project),
        mTopic(topic)
{
}

GetTopicRequest::~GetTopicRequest()
{
}

std::string GetTopicRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string GetTopicRequest::SerializePayload() const
{
    std::string body;
    return body;
}

} // namespace datahub
} // namespace aliyun
