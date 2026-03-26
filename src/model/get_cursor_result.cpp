#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetCursorResult::GetCursorResult()
        : mCursor(),
          mTimestamp(0),
          mSequence(0)
{
}

GetCursorResult::~GetCursorResult()
{
}

void GetCursorResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator cursorItr = doc.FindMember("Cursor");
    if (cursorItr != doc.MemberEnd() && cursorItr->value.IsString())
    {
        mCursor.assign(cursorItr->value.GetString(), cursorItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator timestampItr = doc.FindMember("RecordTime");
    if (timestampItr != doc.MemberEnd() && timestampItr->value.IsInt64())
    {
        mTimestamp = timestampItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator sequenceItr = doc.FindMember("Sequence");
    if (sequenceItr != doc.MemberEnd() && sequenceItr->value.IsInt64())
    {
        mSequence = sequenceItr->value.GetInt64();
    }
}

std::string GetCursorResult::GetCursor() const
{
    return mCursor;
}

int64_t GetCursorResult::GetTimestamp() const
{
    return mTimestamp;
}

int64_t GetCursorResult::GetSequence() const
{
    return mSequence;
}

} // namespace datahub
} // namespace aliyun
