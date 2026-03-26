#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;

namespace aliyun
{
namespace datahub
{

ListConnectorResult::ListConnectorResult()
{
}

ListConnectorResult::~ListConnectorResult()
{
}

void ListConnectorResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator connectorsItr = doc.FindMember("Connectors");
    if (connectorsItr != doc.MemberEnd() && connectorsItr->value.IsArray())
    {
        const rapidjson::Value& connectorIds = connectorsItr->value;
        for (rapidjson::SizeType i = 0; i < connectorIds.Size(); ++i)
        {
            std::string connectorId = connectorIds[i].GetString();
            mConnectorIds.push_back(connectorId);
        }
    }
}

const StringVec ListConnectorResult::GetConnectorIds() const
{
    return mConnectorIds;
}

} // namespace datahub
} // namespace aliyun
