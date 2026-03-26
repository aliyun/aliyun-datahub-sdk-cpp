#include <http/uri.h>

namespace aliyun
{
namespace datahub
{
namespace http
{

URI::URI(const std::string& endpoint) :
    mEndpoint(endpoint)
{
}

URI::~URI()
{
}

void URI::SetPath(const std::string& value)
{
   mPath = value;
}

const std::string& URI::GetPath() const
{
    return mPath;
}

std::string URI::GetURIString() const
{
    return mEndpoint + mPath;
}

} // namespace http
} // namespace datahub
} // namespace aliyun
