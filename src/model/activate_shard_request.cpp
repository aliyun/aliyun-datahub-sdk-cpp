#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"

namespace aliyun
{
namespace datahub
{

ActivateShardRequest::ActivateShardRequest(
        const std::string& project,
        const std::string& topic,
        const StringVec& shards) :
        mProject(project),
        mTopic(topic),
        mShards(shards)
{
}

ActivateShardRequest::~ActivateShardRequest()
{
}

std::string ActivateShardRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards");
    return path;
}

std::string ActivateShardRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;

    jsonDoc.SetObject();

    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action("activate");
    rapidjson::Value shards(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator it;
    for (it = mShards.begin(); it != mShards.end(); ++it)
    {
        rapidjson::Value str;
        if (!it->empty())
        {
            str.SetString(it->c_str(), it->size(), allocator);
            shards.PushBack(str, allocator);
        }
    }

    jsonDoc.AddMember("Action", action, allocator);
    jsonDoc.AddMember("ShardIds", shards, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun

