#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

CreateTopicRequest::CreateTopicRequest(
        const std::string& project,
        const std::string& topic,
        int shardCount,
        int lifeCycle,
        const RecordType& type,
        const RecordSchema& schema,
        const std::string& comment) :
        mProject(project),
        mTopic(topic),
        mShardCount(shardCount),
        mLifeCycle(lifeCycle),
        mRecordType(type),
        mRecordSchema(schema),
        mComment(comment)
{
}

CreateTopicRequest::CreateTopicRequest(
        const std::string& project,
        const std::string& topic,
        int shardCount,
        int lifeCycle,
        const RecordType& type,
        const std::string& comment) :
        mProject(project),
        mTopic(topic),
        mShardCount(shardCount),
        mLifeCycle(lifeCycle),
        mRecordType(type),
        mComment(comment)
{
}

CreateTopicRequest::~CreateTopicRequest()
{
}

std::string CreateTopicRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic);
    return path;
}

std::string CreateTopicRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("ShardCount");
    writer.Int(mShardCount);
    writer.Key("Lifecycle");
    writer.Int(mLifeCycle);
    writer.Key("RecordType");
    writer.String(GetNameForRecordType(mRecordType).c_str());
    if (mRecordType == TUPLE)
    {
        writer.Key("RecordSchema");
        writer.String(mRecordSchema.ToJsonString().c_str());
    }
    writer.Key("Comment");
    writer.String(mComment.c_str());
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
