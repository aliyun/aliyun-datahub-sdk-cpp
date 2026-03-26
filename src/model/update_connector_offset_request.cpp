#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

UpdateConnectorOffsetRequest::UpdateConnectorOffsetRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::ConnectorOffset& connectorOffset) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId),
        mAction("updateshardcontext"),
        mConnectorOffset(connectorOffset)
{
}

UpdateConnectorOffsetRequest::UpdateConnectorOffsetRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId,
        const sdk::ConnectorOffset& connectorOffset) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId),
        mAction("updateshardcontext"),
        mShardId(shardId),
        mConnectorOffset(connectorOffset)
{
}

UpdateConnectorOffsetRequest::~UpdateConnectorOffsetRequest()
{
}

std::string UpdateConnectorOffsetRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/").append(mConnectorId);
    return path;
}

std::string UpdateConnectorOffsetRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    if (!mShardId.empty())
    {
        writer.Key("ShardId");
        writer.String(mShardId.c_str());
    }
    writer.Key("CurrentTime");
    writer.Int64(mConnectorOffset.GetTimestamp());
    writer.Key("CurrentSequence");
    writer.Int64(mConnectorOffset.GetSequence());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
