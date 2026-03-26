#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

SplitShardRequest::SplitShardRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& splitKey) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mSplitKey(splitKey)
{
}

SplitShardRequest::~SplitShardRequest()
{
}

std::string SplitShardRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards");
    return path;
}

std::string SplitShardRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;

    jsonDoc.SetObject();

    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action("split");
    rapidjson::Value shardId(rapidjson::kStringType);
    shardId.SetString(mShardId.c_str(), allocator);
    rapidjson::Value splitKey(rapidjson::kStringType);
    splitKey.SetString(mSplitKey.c_str(), allocator);

    jsonDoc.AddMember("Action", action, allocator);
    jsonDoc.AddMember("ShardId", shardId, allocator);
    jsonDoc.AddMember("SplitKey", splitKey, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
