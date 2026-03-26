#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

MergeShardResult::MergeShardResult()
{
}

MergeShardResult::~MergeShardResult()
{
}

void MergeShardResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value& shardJson = doc;
    if (shardJson.IsObject())
    {
        JsonTool::JsonToShardDesc(shardJson, mShard);
    }
}

const ShardDesc& MergeShardResult::GetChildShard() const
{
    return mShard;
}

} // namespace datahub
} // namespace aliyun
