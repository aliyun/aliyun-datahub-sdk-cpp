#include "datahub/datahub_exception.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetRecordResult::GetRecordResult() :
    mNextCursor(),
    mRecordCount(0),
    mRecords()
{
}

GetRecordResult::~GetRecordResult()
{
}

int GetRecordResult::GetRecordCount() const
{
    return mRecordCount;
}

const RecordResult& GetRecordResult::GetRecord(int index) const
{
    if (index < 0 || index >= mRecordCount)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid record index");
    }
    else
    {
        return mRecords[index];
    }
}

const std::vector<RecordResult>& GetRecordResult::GetRecords() const
{
    return mRecords;
}

void GetRecordResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator nextCursorItr = doc.FindMember("NextCursor");
    if (nextCursorItr != doc.MemberEnd() && nextCursorItr->value.IsString())
    {
        mNextCursor.assign(nextCursorItr->value.GetString(), nextCursorItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator recordCountItr = doc.FindMember("RecordCount");
    if (recordCountItr != doc.MemberEnd() && recordCountItr->value.IsInt())
    {
        mRecordCount = recordCountItr->value.GetInt();
    }

    rapidjson::Value::ConstMemberIterator recordsItr = doc.FindMember("Records");
    if (recordsItr != doc.MemberEnd() && recordsItr->value.IsArray())
    {
        const rapidjson::Value& recordsJson = recordsItr->value;
        for (rapidjson::SizeType i = 0; i < recordsJson.Size(); ++i)
        {
            const rapidjson::Value& recordJson = recordsJson[i];
            RecordResult record;
            JsonTool::JsonToRecordResult(recordJson, record);
            mRecords.push_back(record);
        }
    }
}

std::string GetRecordResult::GetNextCursor() const
{
    return mNextCursor;
}

} // namespace datahub
} // namespace aliyun
