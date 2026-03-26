#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

ListProjectRequest::ListProjectRequest()
{
}

ListProjectRequest::~ListProjectRequest()
{
}

std::string ListProjectRequest::BuildPath() const
{
    std::string path;
    path.append("/projects");
    return path;
}

std::string ListProjectRequest::SerializePayload() const
{
    std::string body;
    return body;
}

} // namespace datahub
} // namespace aliyun
