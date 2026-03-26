#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

UpdateConnectorStateRequest::UpdateConnectorStateRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::ConnectorState& state) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId),
        mAction("updatestate"),
        mState(state)
{
}

UpdateConnectorStateRequest::~UpdateConnectorStateRequest()
{
}

std::string UpdateConnectorStateRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/").append(mConnectorId);
    return path;
}

std::string UpdateConnectorStateRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    writer.Key("State");
    writer.String(GetNameForConnectorState(mState).c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
