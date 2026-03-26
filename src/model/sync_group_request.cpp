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

SyncGroupRequest::SyncGroupRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId,
        const StringVec& releaseShardList,
        const StringVec& readEndShardList) :
        mProject(project),
        mTopic(topic),
        mConsumerGroup(consumerGroup),
        mAction("syncGroup"),
        mConsumerId(consumerId),
        mVersionId(versionId),
        mReleaseShardList(releaseShardList),
        mReadEndShardList(readEndShardList)
{
}

SyncGroupRequest::~SyncGroupRequest()
{
}

std::string SyncGroupRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions/").append(mConsumerGroup);
    return path;
}

std::string SyncGroupRequest::SerializePayload() const
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

    rapidjson::Value releaseShardList(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator rslItr;
    for (rslItr = mReleaseShardList.begin(); rslItr != mReleaseShardList.end(); ++rslItr)
    {
        rapidjson::Value str;
        str.SetString(rslItr->c_str(), rslItr->size(), allocator);
        releaseShardList.PushBack(str, allocator);
    }
    doc.AddMember("ReleaseShardList", releaseShardList, allocator);

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
