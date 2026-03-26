#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_exception.h"

using namespace rapidjson;
namespace aliyun
{
namespace datahub
{

ListProjectResult::ListProjectResult()
{
}

ListProjectResult::~ListProjectResult()
{
}

void ListProjectResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    if (doc.ParseInsitu<kParseValidateEncodingFlag | kParseStopWhenDoneFlag>(const_cast<char *>(payload.c_str())).HasParseError())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Invalid response content: " + payload, mRequestId);
    }

    rapidjson::Value::ConstMemberIterator pnItr = doc.FindMember("ProjectNames");
    if (pnItr != doc.MemberEnd() && pnItr->value.IsArray())
    {
        const rapidjson::Value& projectNames = pnItr->value;
        for (rapidjson::SizeType i = 0; i < projectNames.Size(); ++i)
        {
            std::string projectName = projectNames[i].GetString();
            mProjectNames.push_back(projectName);
        }
    }
}

const StringVec ListProjectResult::GetProjectNames() const
{
    return mProjectNames;
}

} // namespace datahub
} // namespace aliyun
