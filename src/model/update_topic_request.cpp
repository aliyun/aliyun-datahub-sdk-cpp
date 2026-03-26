#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

UpdateTopicRequest::UpdateTopicRequest(
        const std::string& project,
        const std::string& topic,
        int lifeCycle,
        const std::string& comment) :
        mProject(project),
        mTopic(topic),
        mLifeCycle(lifeCycle),
        mComment(comment)
{
}

UpdateTopicRequest::~UpdateTopicRequest()
{
}

std::string UpdateTopicRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string UpdateTopicRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Lifecycle");
    writer.Int(mLifeCycle);
    writer.Key("Comment");
    writer.String(mComment.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
