#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

ListTopicResult::ListTopicResult()
{
}

ListTopicResult::~ListTopicResult()
{
}

void ListTopicResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator tnItr = doc.FindMember("TopicNames");
    if (tnItr != doc.MemberEnd() && tnItr->value.IsArray())
    {
        const rapidjson::Value& topicNames = tnItr->value;
        for (rapidjson::SizeType i = 0; i < topicNames.Size(); ++i)
        {
            std::string topicName = topicNames[i].GetString();
            mTopicNames.push_back(topicName);
        }
    }
}

const StringVec ListTopicResult::GetTopicNames() const
{
    return mTopicNames;
}

} // namespace datahub
} // namespace aliyun
