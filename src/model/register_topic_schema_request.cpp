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

RegisterTopicSchemaRequest::RegisterTopicSchemaRequest(
    const std::string& project,
    const std::string& topic,
    const RecordSchema& schema) :
    mProject(project),
    mTopic(topic),
    mAction("RegisterSchema"),
    mRecordSchema(schema)
{
}

RegisterTopicSchemaRequest::~RegisterTopicSchemaRequest()
{
}

std::string RegisterTopicSchemaRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string RegisterTopicSchemaRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    jsonDoc.AddMember("Action", action, allocator);

    rapidjson::Value recordSchema(rapidjson::kStringType);
    recordSchema.SetString(mRecordSchema.ToJsonString().c_str(), allocator);
    jsonDoc.AddMember("RecordSchema", recordSchema, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun