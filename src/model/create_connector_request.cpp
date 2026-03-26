#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"
#include "json_tool.h"
#include "utils.h"

namespace aliyun
{
namespace datahub
{

CreateConnectorRequest::CreateConnectorRequest(
        const std::string& project,
        const std::string& topic,
        const sdk::ConnectorType& type,
        int64_t sinkStartTime,
        const StringVec& columnFields,
        const sdk::SinkConfig& config) :
        mProject(project),
        mTopic(topic),
        mAction("Create"),
        mType(type),
        mSinkStartTime(sinkStartTime),
        mColumnFields(columnFields),
        mConfig(&config)
{
}

CreateConnectorRequest::CreateConnectorRequest(
        const std::string& project,
        const std::string& topic,
        const sdk::ConnectorType& type,
        const StringVec& columnFields,
        const sdk::SinkConfig& config) :
        mProject(project),
        mTopic(topic),
        mAction("Create"),
        mType(type),
        mSinkStartTime(-1),
        mColumnFields(columnFields),
        mConfig(&config)
{
}

CreateConnectorRequest::~CreateConnectorRequest()
{
}

std::string CreateConnectorRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/connectors/")
    .append(Utils::ToLowerCaseString(GetNameForConnectorType(mType)));
    return path;
}

std::string CreateConnectorRequest::SerializePayload() const
{
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    doc.AddMember("Action", action, allocator);

    rapidjson::Value type(rapidjson::kStringType);
    type.SetString(GetNameForConnectorType(mType).c_str(), allocator);
    doc.AddMember("Type", type, allocator);

    rapidjson::Value sinkStartTime(rapidjson::kNumberType);
    sinkStartTime.SetInt64(mSinkStartTime);
    doc.AddMember("SinkStartTime", sinkStartTime, allocator);

    rapidjson::Value columnFields(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator itr;
    for (itr = mColumnFields.begin(); itr != mColumnFields.end(); ++itr)
    {
        rapidjson::Value columnField(rapidjson::kStringType);
        columnField.SetString(itr->c_str(), itr->size(), allocator);
        columnFields.PushBack(columnField, allocator);
    }
    doc.AddMember("ColumnFields", columnFields, allocator);

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
