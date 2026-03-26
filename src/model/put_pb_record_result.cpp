#include "protobuf_parser.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"
#include "protobuf_tool.h"

namespace aliyun
{
namespace datahub
{

PutPBRecordResult::PutPBRecordResult() :
   PutRecordResult()
{
}

PutPBRecordResult::~PutPBRecordResult()
{
}

void PutPBRecordResult::DeserializePayload(const std::string& payload)
{
    int64_t size = payload.length();
    if (size == 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "No data found in request body");
    }

    aliyun::datahub::proto::PutRecordsResponse putRecordsResponse;
    std::string errMsg;
    if (!ProtobufParser::ParsePbFromArrayBuff(&putRecordsResponse, payload.c_str(), size, errMsg))
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Parse protobuf fail");
    }

    if (putRecordsResponse.has_failed_count())
    {
        mFailedRecordCount = putRecordsResponse.failed_count();
    }

    if (putRecordsResponse.has_failed_count())
    {
        for (int i = 0; i < putRecordsResponse.failed_records_size(); ++i)
        {
            const aliyun::datahub::proto::FailedRecord& failedRecord = putRecordsResponse.failed_records(i);
            ErrorEntry entry;
            ProtobufTool::MessageToErrorEntry(failedRecord, entry);
            mErrorEntries.push_back(entry);
        }
    }
}

} // namespace datahub
} // namespace aliyun
