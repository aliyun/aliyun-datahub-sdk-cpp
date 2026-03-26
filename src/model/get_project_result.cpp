#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetProjectResult::GetProjectResult(const std::string& project) :
        mProject(project),
        mCreateTime(0),
        mLastModifyTime(0)
{
}

GetProjectResult::~GetProjectResult()
{
}

void GetProjectResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator commentItr = doc.FindMember("Comment");
    if (commentItr != doc.MemberEnd() && commentItr->value.IsString())
    {
        mComment.assign(commentItr->value.GetString(), commentItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator creatorItr = doc.FindMember("Creator");
    if (creatorItr != doc.MemberEnd() && creatorItr->value.IsString())
    {
        mCreator.assign(creatorItr->value.GetString(), creatorItr->value.GetStringLength());
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
}

std::string GetProjectResult::GetProject() const
{
    return mProject;
}

std::string GetProjectResult::GetComment() const
{
    return mComment;
}

std::string GetProjectResult::GetCreator() const
{
    return mCreator;
}

int64_t GetProjectResult::GetCreateTime() const
{
    return mCreateTime;
}

int64_t GetProjectResult::GetLastModifyTime() const
{
    return mLastModifyTime;
}

} // namespace datahub
} // namespace aliyun
