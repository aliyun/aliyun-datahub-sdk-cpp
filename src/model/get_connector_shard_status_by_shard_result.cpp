#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetConnectorShardStatusByShardResult::GetConnectorShardStatusByShardResult()
{
}

GetConnectorShardStatusByShardResult::~GetConnectorShardStatusByShardResult()
{
}

void GetConnectorShardStatusByShardResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    mStatusEntry.JsonToShardStatusEntry(doc);
}

const sdk::ConnectorShardStatusEntry& GetConnectorShardStatusByShardResult::GetStatusEntry() const
{
    return mStatusEntry;
}

} // namespace datahub
} // namespace aliyun
