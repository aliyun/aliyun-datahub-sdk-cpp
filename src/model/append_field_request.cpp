#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

AppendFieldRequest::AppendFieldRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& fieldName,
        const std::string& fieldType) :
        mProject(project),
        mTopic(topic),
        mAction("AppendField"),
        mFieldName(fieldName),
        mFieldType(fieldType)
{
}

AppendFieldRequest::~AppendFieldRequest()
{
}

std::string AppendFieldRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string AppendFieldRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    writer.Key("FieldName");
    writer.String(mFieldName.c_str());
    writer.Key("FieldType");
    writer.String(mFieldType.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
