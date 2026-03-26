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

PutPBRecordRequest::PutPBRecordRequest(
        const std::string& project,
        const std::string& topic,
        const std::vector<RecordEntry>& records) :
        PutRecordRequest(project, topic, records)
{
}

PutPBRecordRequest::~PutPBRecordRequest()
{
}

StringMap PutPBRecordRequest::GetRequestSpecificHeaders() const
{
    std::map<std::string, std::string> headers;
    headers[http::REQUEST_ACTION] = "pub";
    headers[http::CONTENT_TYPE_HEADER] = http::CONTENT_TYPE_PROTOBUF;
    return headers;
}

std::string PutPBRecordRequest::SerializePayload() const
{
    aliyun::datahub::proto::PutRecordsRequest putRecordsRequest;
    std::vector<RecordEntry>::const_iterator iter;
    for (iter = mRecords.begin(); iter != mRecords.end(); ++iter)
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
