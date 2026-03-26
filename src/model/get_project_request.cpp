#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

GetProjectRequest::GetProjectRequest(
        const std::string& project) :
        mProject(project)
{
}

GetProjectRequest::~GetProjectRequest()
{
}

std::string GetProjectRequest::SerializePayload() const
{
    std::string body;
    return body;
}

std::string GetProjectRequest::BuildPath() const
{
    std::string path("/projects/" + mProject);
    return path;
}

} // namespace datahub
} // namespace aliyun
