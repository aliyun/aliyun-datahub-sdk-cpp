#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

MergeShardRequest::MergeShardRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& adjacentShardId) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mAdjacentShardId(adjacentShardId)
{
}

MergeShardRequest::~MergeShardRequest()
{
}

std::string MergeShardRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards");;
    return path;
}

std::string MergeShardRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;

    jsonDoc.SetObject();

    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action("merge");
    rapidjson::Value shardId(rapidjson::kStringType);
    shardId.SetString(mShardId.c_str(), allocator);
    rapidjson::Value adjacentShardId(rapidjson::kStringType);
    adjacentShardId.SetString(mAdjacentShardId.c_str(), allocator);

    jsonDoc.AddMember("Action", action, allocator);
    jsonDoc.AddMember("ShardId", shardId, allocator);
    jsonDoc.AddMember("AdjacentShardId", adjacentShardId, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
