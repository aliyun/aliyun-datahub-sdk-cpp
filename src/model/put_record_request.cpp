#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"
#include "json_tool.h"

namespace aliyun
{
namespace datahub
{

PutRecordRequest::PutRecordRequest(
        const std::string& project,
        const std::string& topic,
        const std::vector<RecordEntry>& records) :
        mProject(project),
        mTopic(topic),
        mRecords(records)
{
}

PutRecordRequest::~PutRecordRequest()
{
}

std::string PutRecordRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards");;
    return path;
}

std::string PutRecordRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;

    jsonDoc.SetObject();

    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action("pub");

    jsonDoc.AddMember("Action", action, allocator);

    rapidjson::Value jsonArray(rapidjson::kArrayType);
    for (auto iter = mRecords.begin(); iter != mRecords.end(); ++iter)
    {
        rapidjson::Value jsonValue(rapidjson::kObjectType);
        JsonTool::RecordEntryToJson((*iter), allocator, jsonValue);
        jsonArray.PushBack(jsonValue, allocator);
    }

    jsonDoc.AddMember("Records", jsonArray, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
