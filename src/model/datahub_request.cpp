#include "datahub/datahub_request.h"
#include "utils.h"

namespace aliyun
{
namespace datahub
{

std::string DatahubRequest::GetPath() const
{
    std::string path = BuildPath();

    StringMap parameters = GetRequestParameters();
    if (!parameters.empty())
    {
        path.append("?");
        std::map<std::string, std::string>::const_iterator iter;
        for (iter = parameters.begin(); iter != parameters.end(); ++iter)
        {
            if (iter != parameters.begin())
            {
                path.append("&");
            }

            path.append(iter->first);
            path.append("=");
            path.append(Utils::UrlEncode(iter->second));
        }
    }

    return path;
}

std::string DatahubRequest::GetBody() const
{
    return SerializePayload();
}

StringMap DatahubRequest::GetHeaders() const
{
    return GetRequestSpecificHeaders();
}

StringMap DatahubRequest::GetRequestParameters() const
{
    StringMap parameters;
    return parameters;
}

StringMap DatahubRequest::GetRequestSpecificHeaders() const
{
    StringMap headers;
    return headers;
}

} // namespace datahub
} // namespace aliyun
