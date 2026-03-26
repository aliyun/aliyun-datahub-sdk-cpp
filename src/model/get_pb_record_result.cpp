#include "protobuf_parser.h"
#include "datahub/datahub_exception.h"
#include "datahub/datahub_result.h"
#include "protobuf_tool.h"

namespace aliyun
{
namespace datahub
{

GetPBRecordResult::GetPBRecordResult() :
    GetRecordResult()
{
}

GetPBRecordResult::~GetPBRecordResult()
{
}

void GetPBRecordResult::DeserializePayload(const std::string& payload)
{
    int64_t size = payload.length();
    if (size == 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "No data found in request body");
    }
    aliyun::datahub::proto::GetRecordsResponse getRecordsResponse;
    std::string errMsg;
    if (!ProtobufParser::ParsePbFromArrayBuff(&getRecordsResponse, payload.c_str(), size, errMsg))
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Parse protobuf fail");
    }

    mNextCursor = getRecordsResponse.next_cursor();
    mRecordCount = getRecordsResponse.record_count();

    for (int i = 0; i < getRecordsResponse.records_size(); ++i)
    {
        RecordResult record;
        ProtobufTool::MessageToRecordResult(getRecordsResponse.records(i), record);
        mRecords.push_back(record);
    }
}

} // namespace datahub
} // namespace aliyun
