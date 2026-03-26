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

HeartbeatRequest::HeartbeatRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId,
        const StringVec& holdShardList,
        const StringVec& readEndShardList) :
        mProject(project),
        mTopic(topic),
        mConsumerGroup(consumerGroup),
        mAction("heartbeat"),
        mConsumerId(consumerId),
        mVersionId(versionId),
        mHoldShardList(holdShardList),
        mReadEndShardList(readEndShardList)
{
}

HeartbeatRequest::~HeartbeatRequest()
{
}

std::string HeartbeatRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mConsumerGroup);
    return path;
}

std::string HeartbeatRequest::SerializePayload() const
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

    rapidjson::Value holdShardList(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator hslItr;
    for (hslItr = mHoldShardList.begin(); hslItr != mHoldShardList.end(); ++hslItr)
    {
        rapidjson::Value str;
        str.SetString(hslItr->c_str(), hslItr->size(), allocator);
        holdShardList.PushBack(str, allocator);
    }
    doc.AddMember("HoldShardList", holdShardList, allocator);

    rapidjson::Value readEndShardList(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator reslItr;
    for (reslItr = mReadEndShardList.begin(); reslItr != mReadEndShardList.end(); ++reslItr)
    {
        rapidjson::Value str;
        str.SetString(reslItr->c_str(), reslItr->size(), allocator);
        readEndShardList.PushBack(str, allocator);
    }
    doc.AddMember("ReadEndShardList", readEndShardList, allocator);

    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    doc.Accept(writer);

    return std::string(strbuf.GetString(), strbuf.GetSize());
}

} // namespace datahub
} // namespace aliyun
