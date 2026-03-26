#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetConnectorShardStatusResult::GetConnectorShardStatusResult()
{
}

GetConnectorShardStatusResult::~GetConnectorShardStatusResult()
{
}

void GetConnectorShardStatusResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator ssiItr = doc.FindMember("ShardStatusInfos");
    if (ssiItr != doc.MemberEnd() && ssiItr->value.IsObject())
    {
        const rapidjson::Value& shardStatusInfoJson = ssiItr->value;
        for (rapidjson::Value::ConstMemberIterator infoItr = shardStatusInfoJson.MemberBegin();
             infoItr != shardStatusInfoJson.MemberEnd(); ++infoItr)
        {
            const std::string shardId = infoItr->name.GetString();
            const rapidjson::Value& shardStatusInfo = infoItr->value;
            if (shardStatusInfo.IsObject())
            {
                sdk::ConnectorShardStatusEntry shardStatusEntry;
                shardStatusEntry.JsonToShardStatusEntry(shardStatusInfo);
                mStatusEntryMap.insert(std::pair<std::string, sdk::ConnectorShardStatusEntry>(shardId, shardStatusEntry));
            }
        }
    }
}

const std::map<std::string, sdk::ConnectorShardStatusEntry>& GetConnectorShardStatusResult::GetStatusEntryMap() const
{
    return mStatusEntryMap;
}

} // namespace datahub
} // namespace aliyun
