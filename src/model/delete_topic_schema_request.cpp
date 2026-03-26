#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "json_tool.h"

namespace aliyun
{
namespace datahub
{

DeleteTopicSchemaRequest::DeleteTopicSchemaRequest(
    const std::string& project,
    const std::string& topic,
    int64_t versionId) :
    mProject(project),
    mTopic(topic),
    mAction("DeleteSchema"),
    mVersionId(versionId)
{
}

DeleteTopicSchemaRequest::~DeleteTopicSchemaRequest()
{
}

std::string DeleteTopicSchemaRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string DeleteTopicSchemaRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    jsonDoc.AddMember("Action", action, allocator);

    jsonDoc.AddMember("VersionId", mVersionId, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun