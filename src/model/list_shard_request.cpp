#include <string>
#include <memory>
#include "datahub/datahub_request.h"

namespace aliyun
{
namespace datahub
{

ListShardRequest::ListShardRequest(
        const std::string& project,
        const std::string& topic) :
        mProject(project),
        mTopic(topic)
{
}

ListShardRequest::~ListShardRequest()
{
}

std::string ListShardRequest::BuildPath() const
{
    std::string path;
    path.append("/projects/").append(mProject).append("/topics/").append(mTopic).append("/shards");
    return path;
}

std::string ListShardRequest::SerializePayload() const
{
    std::string body;
    return body;
}

} // namespace datahub
} // namespace aliyun
