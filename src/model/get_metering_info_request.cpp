#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

GetMeteringInfoRequest::GetMeteringInfoRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mAction("meter")
{
}

GetMeteringInfoRequest::~GetMeteringInfoRequest()
{
}

std::string GetMeteringInfoRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards/").append(mShardId);
    return path;
}

std::string GetMeteringInfoRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
