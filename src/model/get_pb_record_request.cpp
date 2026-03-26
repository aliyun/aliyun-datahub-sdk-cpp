#include <string>
#include <memory>
#include "protobuf_parser.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"
#include "http/http_request.h"
#include "protobuf_tool.h"

namespace aliyun
{
namespace datahub
{

GetPBRecordRequest::GetPBRecordRequest(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& cursor,
        int limit) :
        GetRecordRequest(project, topic, shardId, cursor, limit)
{
}

GetPBRecordRequest::~GetPBRecordRequest()
{
}

StringMap GetPBRecordRequest::GetRequestSpecificHeaders() const
{
    StringMap headers;
    headers[http::REQUEST_ACTION] = "sub";
    headers[http::CONTENT_TYPE_HEADER] = http::CONTENT_TYPE_PROTOBUF;
    if (!mSubId.empty())
    {
        headers[http::REQUEST_SUB_ID] = mSubId;
    }
    return headers;
}

std::string GetPBRecordRequest::SerializePayload() const
{
    aliyun::datahub::proto::GetRecordsRequest getRecordsRequest;
    getRecordsRequest.set_cursor(mCursor);
    getRecordsRequest.set_limit(mLimit);
    if (mFilter.empty())
    {
        getRecordsRequest.set_filter(mFilter);
    }

    std::string payload;
    ProtobufParser::ParsePbToString(getRecordsRequest, payload);
    return payload;
}

} // namespace datahub
} // namespace aliyun
