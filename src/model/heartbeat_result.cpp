#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

HeartbeatResult::HeartbeatResult() :
        mPlanVersion(0)
{
}

HeartbeatResult::~HeartbeatResult()
{
}

void HeartbeatResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator planVersionItr = doc.FindMember("PlanVersion");
    if (planVersionItr != doc.MemberEnd() && planVersionItr->value.IsInt64())
    {
        mPlanVersion = planVersionItr->value.GetInt64();
    }

    rapidjson::Value::ConstMemberIterator totalPlanItr = doc.FindMember("TotalPlan");
    if (totalPlanItr != doc.MemberEnd() && totalPlanItr->value.IsString())
    {
        mTotalPlan.assign(totalPlanItr->value.GetString(), totalPlanItr->value.GetStringLength());
    }

    rapidjson::Value::ConstMemberIterator slItr = doc.FindMember("ShardList");
    if (slItr != doc.MemberEnd() && slItr->value.IsArray())
    {
        const rapidjson::Value& shardList = slItr->value;
        for (rapidjson::SizeType i = 0; i < shardList.Size(); ++i)
        {
            std::string shard = shardList[i].GetString();
            mShardList.push_back(shard);
        }
    }
}

int64_t HeartbeatResult::GetPlanVersion() const
{
    return mPlanVersion;
}

const StringVec HeartbeatResult::GetShardList() const
{
    return mShardList;
}

std::string HeartbeatResult::GetTotalPlan() const
{
    return mTotalPlan;
}

} // namespace datahub
} // namespace aliyun
