#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "http/http_request.h"

namespace aliyun
{
namespace datahub
{

GetRecordRequest::GetRecordRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& cursor, int limit) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mCursor(cursor),
        mLimit(limit)
{
}

GetRecordRequest::~GetRecordRequest()
{
}

std::string GetRecordRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards/").append(mShardId);
    return path;
}

StringMap GetRecordRequest::GetRequestSpecificHeaders() const
{
    StringMap headers;
    if (!mSubId.empty())
    {
        headers[http::REQUEST_SUB_ID] = mSubId;
    }
    return headers;
}

std::string GetRecordRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;

    jsonDoc.SetObject();

    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action("sub");
    jsonDoc.AddMember("Action", action, allocator);

    rapidjson::Value cursorObjString(rapidjson::kStringType);
    cursorObjString.SetString(mCursor.c_str(), allocator);
    jsonDoc.AddMember("Cursor", cursorObjString, allocator);

    jsonDoc.AddMember("Limit", mLimit, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
