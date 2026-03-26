#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

DeleteProjectRequest::DeleteProjectRequest(
        const std::string& project) :
        mProject(project)
{
}

DeleteProjectRequest::~DeleteProjectRequest()
{
}

std::string DeleteProjectRequest::SerializePayload() const
{
    std::string body;
    return body;
}

std::string DeleteProjectRequest::BuildPath() const
{
    std::string path("/projects/" + mProject);
    return path;
}

} // namespace datahub
} // namespace aliyun
