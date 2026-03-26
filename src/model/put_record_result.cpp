#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"
#include "json_tool.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

PutRecordResult::PutRecordResult() :
    mFailedRecordCount(-1)
{
}

PutRecordResult::~PutRecordResult()
{
}

void PutRecordResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator frcItr = doc.FindMember("FailedRecordCount");
    if (frcItr != doc.MemberEnd() && frcItr->value.IsInt())
    {
        mFailedRecordCount = frcItr->value.GetInt();
    }

    rapidjson::Value::ConstMemberIterator frItr = doc.FindMember("FailedRecords");
    if (frItr != doc.MemberEnd() && frItr->value.IsArray())
    {
        const rapidjson::Value& errorJsons = frItr->value;

        for (rapidjson::SizeType i = 0; i < errorJsons.Size(); ++i)
        {
            const rapidjson::Value& errorJson = errorJsons[i];
            ErrorEntry entry;
            JsonTool::JsonToErrorEntry(errorJson, entry);
            mErrorEntries.push_back(entry);
        }
    }
}

int PutRecordResult::GetFailedRecordCount() const
{
    return mFailedRecordCount;
}

const std::vector<ErrorEntry>& PutRecordResult::GetErrorEntries() const
{
    return mErrorEntries;
}

void PutRecordResult::SetFailedRecords(const std::vector<RecordEntry>& failedRecords)
{
    mFailedRecords = failedRecords;
}

void PutRecordResult::SetFailedRecords(std::vector<RecordEntry>&& failedRecords)
{
    mFailedRecords = std::move(failedRecords);
}

const std::vector<RecordEntry>& PutRecordResult::GetFailedRecords() const
{
    return mFailedRecords;
}

} // namespace datahub
} // namespace aliyun
