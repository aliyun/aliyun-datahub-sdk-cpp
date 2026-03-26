#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

GetCursorRequest::GetCursorRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const CursorType& type) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mType(type),
        mValue(0)
{
}

GetCursorRequest::GetCursorRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        long value,
        const CursorType& type) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mType(type),
        mValue(value)
{
}

GetCursorRequest::~GetCursorRequest()
{
}

std::string GetCursorRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards/").append(mShardId);
    return path;
}

std::string GetCursorRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;

    jsonDoc.SetObject();

    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action("cursor");

    jsonDoc.AddMember("Action", action, allocator);

    if (mType == CURSOR_TYPE_TIMESTAMP)
    {
        jsonDoc.AddMember("SystemTime", mValue, allocator);
    }
    else if (mType == CURSOR_TYPE_SEQUENCE)
    {
        jsonDoc.AddMember("Sequence", mValue, allocator);
    }

    rapidjson::Value typeObjString(rapidjson::kStringType);
    typeObjString.SetString(GetNameForCursorType(mType).c_str(), allocator);
    jsonDoc.AddMember("Type", typeObjString, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
