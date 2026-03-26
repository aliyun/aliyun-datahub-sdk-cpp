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

UpdateConnectorRequest::UpdateConnectorRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::SinkConfig& config) :
        mProject(project),
        mTopic(topic),
        mConnectorId(connectorId),
        mAction("updateconfig"),
        mConfig(&config)
{
}

UpdateConnectorRequest::~UpdateConnectorRequest()
{
}

std::string UpdateConnectorRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/").append(mConnectorId);
    return path;
}

std::string UpdateConnectorRequest::SerializePayload() const
{
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    doc.AddMember("Action", action, allocator);

    rapidjson::Value configJson(rapidjson::kObjectType);
    mConfig->SinkConfigToJson(allocator, configJson);
    doc.AddMember("Config", configJson, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
