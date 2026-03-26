#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

CreateProjectRequest::CreateProjectRequest(
        const std::string& project,
        const std::string& comment) :
        mProject(project),
        mComment(comment)
{
}

CreateProjectRequest::~CreateProjectRequest()
{
}

std::string CreateProjectRequest::SerializePayload() const
{
    std::string body("{\"Comment\": \"" + mComment + "\"}");
    return body;
}

std::string CreateProjectRequest::BuildPath() const
{
    std::string path("/projects/" + mProject);
    return path;
}

} // namespace datahub
} // namespace aliyun
