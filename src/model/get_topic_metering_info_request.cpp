#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "utils.h"

namespace aliyun
{
namespace datahub
{

GetTopicMeteringInfoRequest::GetTopicMeteringInfoRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& day)
        : mProject(project),
          mTopic(topic),
          mDay(day),
          mAction("meter")
{
}

GetTopicMeteringInfoRequest::~GetTopicMeteringInfoRequest()
{
}

std::string GetTopicMeteringInfoRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string GetTopicMeteringInfoRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());

    uint64_t timestamp = Utils::ConvertToTimestamp(mDay);
    writer.Key("Timestamp");
    writer.Uint64(timestamp);

    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
