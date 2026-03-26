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

LeaveGroupRequest::LeaveGroupRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId) :
        mProject(project),
        mTopic(topic),
        mConsumerGroup(consumerGroup),
        mAction("leaveGroup"),
        mConsumerId(consumerId),
        mVersionId(versionId)
{
}

LeaveGroupRequest::~LeaveGroupRequest()
{
}

std::string LeaveGroupRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mConsumerGroup);
    return path;
}

std::string LeaveGroupRequest::SerializePayload() const
{
    rapidjson::Document doc;
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.SetObject();

    rapidjson::Value action(rapidjson::kStringType);
    action.SetString(mAction.c_str(), allocator);
    doc.AddMember("Action", action, allocator);

    rapidjson::Value consumerId(rapidjson::kStringType);
    consumerId.SetString(mConsumerId.c_str(), allocator);
    doc.AddMember("ConsumerId", consumerId, allocator);

    rapidjson::Value versionId(rapidjson::kStringType);
    versionId.SetInt64(mVersionId);
    doc.AddMember("VersionId", versionId, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
