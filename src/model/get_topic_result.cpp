#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetTopicResult::GetTopicResult() :
    mShardCount(0),
    mLifecycle(0),
    mCreateTime(0),
    mLastModifyTime(0)
{
}

GetTopicResult::~GetTopicResult()
{
}

void GetTopicResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator shardCountItr = doc.FindMember("ShardCount");
    if (shardCountItr != doc.MemberEnd() && shardCountItr->value.IsInt())
    {
        mShardCount = shardCountItr->value.GetInt();
    }

    rapidjson::Value::ConstMemberIterator lifecycleItr = doc.FindMember("Lifecycle");
    if (lifecycleItr != doc.MemberEnd() && lifecycleItr->value.IsInt())
    {
        mLifecycle = lifecycleItr->value.GetInt();
    }

    rapidjson::Value::ConstMemberIterator recordTypeItr = doc.FindMember("RecordType");
    if (recordTypeItr != doc.MemberEnd() && recordTypeItr->value.IsString())
    {
        mRecordType.assign(recordTypeItr->value.GetString(), recordTypeItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator createTimeItr = doc.FindMember("CreateTime");
    if (createTimeItr != doc.MemberEnd() && createTimeItr->value.IsInt())
    {
        mCreateTime = createTimeItr->value.GetInt() * 1000l;
    }

    rapidjson::Value::ConstMemberIterator lastModifyTimeItr = doc.FindMember("LastModifyTime");
    if (lastModifyTimeItr != doc.MemberEnd() && lastModifyTimeItr->value.IsInt())
    {
        mLastModifyTime = lastModifyTimeItr->value.GetInt() * 1000l;
    }

    rapidjson::Value::ConstMemberIterator commentItr = doc.FindMember("Comment");
    if (commentItr != doc.MemberEnd() && commentItr->value.IsString())
    {
        mComment.assign(commentItr->value.GetString(), commentItr->value.GetStringLength());
    }

    if (mRecordType == "TUPLE")
    {
        rapidjson::Value::ConstMemberIterator recordSchemaItr = doc.FindMember("RecordSchema");
        if (recordSchemaItr != doc.MemberEnd() && recordSchemaItr->value.IsString())
        {
            const std::string& jsonString = recordSchemaItr->value.GetString();
            mRecordSchema.FromJsonString(jsonString);
        }
    }
}

int GetTopicResult::GetShardCount() const
{
    return mShardCount;
}

int GetTopicResult::GetLifecycle() const
{
    return mLifecycle;
}

std::string GetTopicResult::GetRecordType() const
{
    return mRecordType;
}

const RecordSchema& GetTopicResult::GetRecordSchema() const
{
    return mRecordSchema;
}

std::string GetTopicResult::GetComment() const
{
    return mComment;
}

int64_t GetTopicResult::GetCreateTime() const
{
    return mCreateTime;
}

int64_t GetTopicResult::GetLastModifyTime() const
{
    return mLastModifyTime;
}

} // namespace datahub
} // namespace aliyun
