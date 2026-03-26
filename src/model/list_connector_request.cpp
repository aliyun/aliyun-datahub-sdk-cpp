#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

ListConnectorRequest::ListConnectorRequest(
        const std::string& project,
        const std::string& topic) :
        mProject(project),
        mTopic(topic),
        mMode("id")
{
}

ListConnectorRequest::~ListConnectorRequest()
{
}

std::string ListConnectorRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors");
    // .append("?mode=").append(mMode);
    return path;
}

std::string ListConnectorRequest::SerializePayload() const
{
    std::string body;
    return body;
}

StringMap ListConnectorRequest::GetRequestParameters() const
{
    StringMap parameters;
    parameters["mode"] = mMode;
    return parameters;
}

} // namespace datahub
} // namespace aliyun
