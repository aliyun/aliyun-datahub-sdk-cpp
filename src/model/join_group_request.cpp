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

JoinGroupRequest::JoinGroupRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        int64_t sessionTimeout) :
        mProject(project),
        mTopic(topic),
        mConsumerGroup(consumerGroup),
        mAction("joinGroup"),
        mSessionTimeout(sessionTimeout)
{
}

JoinGroupRequest::~JoinGroupRequest()
{
}

std::string JoinGroupRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mConsumerGroup);
    return path;
}

std::string JoinGroupRequest::SerializePayload() const
{
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    doc.AddMember("Action", action, allocator);

    rapidjson::Value sessionTimeout(rapidjson::kStringType);
    sessionTimeout.SetInt64(mSessionTimeout);
    doc.AddMember("SessionTimeout", sessionTimeout, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
