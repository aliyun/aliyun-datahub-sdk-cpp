#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

ListShardResult::ListShardResult()
{
}

ListShardResult::~ListShardResult()
{
}

void ListShardResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator shardsItr = doc.FindMember("Shards");
    if (shardsItr != doc.MemberEnd() && shardsItr->value.IsArray())
    {
        const rapidjson::Value& shardsJson = shardsItr->value;
        for (rapidjson::SizeType i = 0; i < shardsJson.Size(); ++i)
        {
            const rapidjson::Value& shardJson = shardsJson[i];
            if (shardJson.IsObject())
            {
                ShardEntry shardEntry;
                JsonTool::JsonToShardEntry(shardJson, shardEntry);
                mShards.push_back(shardEntry);
            }
        }
    }
}

const std::vector<ShardEntry>& ListShardResult::GetShards() const
{
    return mShards;
}

} // namespace datahub
} // namespace aliyun
