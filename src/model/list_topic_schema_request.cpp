#include <string>
#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"
#include "json_tool.h"

namespace aliyun
{
namespace datahub
{

ListTopicSchemaRequest::ListTopicSchemaRequest(
    const std::string& project,
    const std::string& topic,
    int pageNumber,
    int pageSize) :
    mProject(project),
    mTopic(topic),
    mAction("ListSchema"),
    mPageNumber(pageNumber),
    mPageSize(pageSize)
{
}

ListTopicSchemaRequest::~ListTopicSchemaRequest()
{
}

std::string ListTopicSchemaRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string ListTopicSchemaRequest::SerializePayload() const
{
    rapidjson::Document jsonDoc;
    jsonDoc.SetObject();
    rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    jsonDoc.AddMember("Action", action, allocator);

    rapidjson::Value pageNumber(rapidjson::kNumberType);
    pageNumber.SetInt(mPageNumber);
    jsonDoc.AddMember("PageNumber", pageNumber, allocator);

    rapidjson::Value pageSize(rapidjson::kNumberType);
    pageSize.SetInt(mPageSize);
    jsonDoc.AddMember("PageSize", pageSize, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    jsonDoc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun