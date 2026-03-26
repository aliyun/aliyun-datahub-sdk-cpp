#include <string>
#include <memory>
#include "protobuf_parser.h"
#include "http/http_request.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"
#include "protobuf_tool.h"

namespace aliyun
{
namespace datahub
{

PutRecordByShardRequest::PutRecordByShardRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::vector<RecordEntry>& records) :
        mProject(project),
        mTopic(topic),
        mShardId(shardId),
        mRecords(records)
{
}

PutRecordByShardRequest::~PutRecordByShardRequest()
{
}

StringMap PutRecordByShardRequest::GetRequestSpecificHeaders() const
{
    std::map<std::string, std::string> headers;
    headers[http::REQUEST_ACTION] = "pub";
    headers[http::CONTENT_TYPE_HEADER] = http::CONTENT_TYPE_PROTOBUF;
    return headers;
}

std::string PutRecordByShardRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards/").append(mShardId);
    return path;
}

std::string PutRecordByShardRequest::SerializePayload() const
{
    aliyun::datahub::proto::PutRecordsRequest putRecordsRequest;
    for (auto iter = mRecords.begin(); iter != mRecords.end(); ++iter)
    {
        aliyun::datahub::proto::RecordEntry* record = putRecordsRequest.add_records();
        ProtobufTool::RecordEntryToMessage(*iter, record);
    }
    std::string payload;
    ProtobufParser::ParsePbToString(putRecordsRequest, payload);
    return payload;
}

} // namespace datahub
} // namespace aliyun

