#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;

namespace aliyun
{
namespace datahub
{

CreateConnectorResult::CreateConnectorResult()
{
}

CreateConnectorResult::~CreateConnectorResult()
{
}

void CreateConnectorResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator connectorIdItr = doc.FindMember("ConnectorId");
    if (connectorIdItr != doc.MemberEnd() && connectorIdItr->value.IsString())
    {
        mConnectorId.assign(connectorIdItr->value.GetString(), connectorIdItr->value.GetStringLength());
    }
}

std::string CreateConnectorResult::GetConnectorId() const
{
    return mConnectorId;
}

} // namespace datahub
} // namespace aliyun
