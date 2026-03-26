#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "json_tool.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

SplitShardResult::SplitShardResult()
{
}

SplitShardResult::~SplitShardResult()
{
}

void SplitShardResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator newShardsItr = doc.FindMember("NewShards");
    if (newShardsItr != doc.MemberEnd() && newShardsItr->value.IsArray())
    {
        const rapidjson::Value& shardsJson = newShardsItr->value;
        for (rapidjson::SizeType i = 0; i < shardsJson.Size(); ++i)
        {
            const rapidjson::Value& shardJson = shardsJson[i];
            if (shardJson.IsObject())
            {
                ShardDesc shardDesc;
                JsonTool::JsonToShardDesc(shardJson, shardDesc);
                mShards.push_back(shardDesc);
            }
        }
    }
}

const std::vector<ShardDesc>& SplitShardResult::GetChildShards() const
{
    return mShards;
}

} // namespace datahub
} // namespace aliyun
