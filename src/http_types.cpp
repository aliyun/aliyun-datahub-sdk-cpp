#include "http/http_types.h"

namespace aliyun
{
namespace datahub
{
namespace http
{

std::string GetNameForHttpMethod(const HttpMethod& httpMethod)
{
    switch (httpMethod)
    {
        case HTTP_GET:
            return "GET";
        case HTTP_POST:
            return "POST";
        case HTTP_DELETE:
            return "DELETE";
        case HTTP_PUT:
            return "PUT";
        case HTTP_HEAD:
            return "HEAD";
        case HTTP_PATCH:
            return "PATCH";
        default:
            return "GET";
    }
}

} // namespace http
} // namespace datahub
} // namespace aliyun
