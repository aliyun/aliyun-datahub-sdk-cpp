#include <string>
#include <memory>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

ListSubscriptionRequest::ListSubscriptionRequest(
        const std::string& project,
        const std::string& topic,
        int pageIndex,
        int pageSize,
        const std::string& search) :
        mProject(project),
        mTopic(topic),
        mAction("list"),
        mPageIndex(pageIndex),
        mPageSize(pageSize),
        mSearch(search)
{
}

ListSubscriptionRequest::ListSubscriptionRequest(
        const std::string& project,
        const std::string& topic,
        int pageIndex,
        int pageSize) :
        mProject(project),
        mTopic(topic),
        mAction("list"),
        mPageIndex(pageIndex),
        mPageSize(pageSize)
{
}

ListSubscriptionRequest::~ListSubscriptionRequest()
{
}

std::string ListSubscriptionRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/subscriptions");
    return path;
}

std::string ListSubscriptionRequest::SerializePayload() const
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(stringBuffer);
    writer.StartObject();
    writer.Key("Action");
    writer.String(mAction.c_str());
    writer.Key("PageIndex");
    writer.Int(mPageIndex);
    writer.Key("PageSize");
    writer.Int(mPageSize);
    if (mSearch.size() > 0)
    {
        writer.Key("Search");
        writer.String(mSearch.c_str());
    }
    writer.EndObject();

    return std::string(stringBuffer.GetString(), stringBuffer.GetSize());
}

} // namespace datahub
} // namespace aliyun
