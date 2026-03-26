#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

UpdateProjectRequest::UpdateProjectRequest(
        const std::string& project,
        const std::string& comment) :
        mProject(project),
        mComment(comment)
{
}

UpdateProjectRequest::~UpdateProjectRequest()
{
}

std::string UpdateProjectRequest::SerializePayload() const
{
    std::string body("{\"Comment\": \"" + mComment + "\"}");
    return body;
}

std::string UpdateProjectRequest::BuildPath() const
{
    std::string path("/projects/" + mProject);
    return path;
}

} // namespace datahub
} // namespace aliyun
