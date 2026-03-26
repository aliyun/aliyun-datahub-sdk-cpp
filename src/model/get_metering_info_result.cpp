#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

GetMeteringInfoResult::GetMeteringInfoResult() :
        mActiveTime(0),
        mStorage(0)
{
}

GetMeteringInfoResult::~GetMeteringInfoResult()
{
}

void GetMeteringInfoResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator activeTimeItr = doc.FindMember("ActiveTime");
    if (activeTimeItr != doc.MemberEnd() && activeTimeItr->value.IsInt64())
    {
        mActiveTime = activeTimeItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator storageItr = doc.FindMember("Storage");
    if (storageItr != doc.MemberEnd() && storageItr->value.IsInt64())
    {
        mStorage = storageItr->value.GetInt64();
    }
}

int64_t GetMeteringInfoResult::GetActiveTime() const
{
    return mActiveTime;
}

int64_t GetMeteringInfoResult::GetStorage() const
{
    return mStorage;
}

} // namespace datahub
} // namespace aliyun
