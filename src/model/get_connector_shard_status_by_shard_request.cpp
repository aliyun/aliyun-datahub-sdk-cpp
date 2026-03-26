#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

GetConnectorShardStatusByShardRequest::GetConnectorShardStatusByShardRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId),
        mAction("Status"),
        mShardId(shardId)
{
}

GetConnectorShardStatusByShardRequest::~GetConnectorShardStatusByShardRequest()
{
}

std::string GetConnectorShardStatusByShardRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/").append(mConnectorId);
    return path;
}

std::string GetConnectorShardStatusByShardRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    writer.Key("ShardId");
    writer.String(mShardId.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
