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

GetTopicSchemaRequest::GetTopicSchemaRequest(
    const std::string& project,
    const std::string& topic,
    int64_t versionId,
    const RecordSchema& schema) :
    mProject(project),
    mTopic(topic),
    mAction("GetSchema"),
    mVersionId(versionId),
    mRecordSchema(schema)
{
}

GetTopicSchemaRequest::~GetTopicSchemaRequest()
{
}

std::string GetTopicSchemaRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string GetTopicSchemaRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    jsonDoc.AddMember("Action", action, allocator);

    rapidjson::Value recordSchema(rapidjson::kStringType);
    recordSchema.SetString((mRecordSchema.GetFieldCount() > 0) ? mRecordSchema.ToJsonString().c_str() : "", allocator);
    jsonDoc.AddMember("RecordSchema", recordSchema, allocator);

    jsonDoc.AddMember("VersionId", mVersionId, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun