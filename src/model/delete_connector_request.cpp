#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

DeleteConnectorRequest::DeleteConnectorRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId)
{
}

DeleteConnectorRequest::~DeleteConnectorRequest()
{
}

std::string DeleteConnectorRequest::SerializePayload() const
{
    std::string body;
    return body;
}

std::string DeleteConnectorRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/").append(mConnectorId);
    return path;
}

} // namespace datahub
} // namespace aliyun
