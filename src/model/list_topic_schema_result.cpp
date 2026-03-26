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

ListTopicSchemaResult::ListTopicSchemaResult()
{
}

ListTopicSchemaResult::~ListTopicSchemaResult()
{
}

int64_t ListTopicSchemaResult::GetPageNumber() const
{
    return mPageNumber;
}

int64_t ListTopicSchemaResult::GetPageSize() const
{
    return mPageSize;
}

int64_t ListTopicSchemaResult::GetPageCount() const
{
    return mPageCount;
}

int64_t ListTopicSchemaResult::GetTotalCount() const
{
    return mTotalCount;
}

const std::vector<RecordSchema>&  ListTopicSchemaResult::GetRecordSchemaList() const
{
    return mRecordSchemaList;
}

void ListTopicSchemaResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator valueItr = doc.FindMember("PageNumber");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsInt64())
    {
        mPageNumber = valueItr->value.GetInt64();
    }

    valueItr = doc.FindMember("PageSize");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsInt64())
    {
        mPageSize = valueItr->value.GetInt64();
    }

    valueItr = doc.FindMember("PageCount");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsInt64())
    {
        mPageCount = valueItr->value.GetInt64();
    }

    valueItr = doc.FindMember("TotalCount");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsInt64())
    {
        mTotalCount = valueItr->value.GetInt64();
    }

    valueItr = doc.FindMember("RecordSchemaList");
    if (valueItr != doc.MemberEnd() && valueItr->value.IsArray())
    {
        const rapidjson::Value& recordSchemaList = valueItr->value;
        for (rapidjson::SizeType i = 0; i < recordSchemaList.Size(); ++i)
        {
            const rapidjson::Value& recordSchemaJson = recordSchemaList[i];
            if (recordSchemaJson.IsObject())
            {
                RecordSchema recordSchema;
                recordSchema.FromJsonString(recordSchemaJson.FindMember("RecordSchema")->value.GetString());
                mRecordSchemaList.push_back(recordSchema);
            }
        }
    }
}

} // namespace datahub
} // namespace aliyun