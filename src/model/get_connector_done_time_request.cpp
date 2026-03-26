#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

GetConnectorDoneTimeRequest::GetConnectorDoneTimeRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId)
{
}

GetConnectorDoneTimeRequest::~GetConnectorDoneTimeRequest()
{
}

std::string GetConnectorDoneTimeRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/").append(mConnectorId)
    .append("?donetime");
    return path;
}

std::string GetConnectorDoneTimeRequest::SerializePayload() const
{
    std::string body;
    return body;
}

} // namespace datahub
} // namespace aliyun
