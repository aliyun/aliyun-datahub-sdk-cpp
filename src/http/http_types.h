#ifndef DATAHUB_SDK_HTTP_TYPES_H
#define DATAHUB_SDK_HTTP_TYPES_H

#include <memory>

#ifndef typeof
#define typeof(x)   __typeof__(x)
#endif

namespace aliyun
{
namespace datahub
{
namespace http
{

enum TransferLibType
{
    CURL_CLIENT
};

enum HttpMethod
{
    HTTP_GET,
    HTTP_POST,
    HTTP_DELETE,
    HTTP_PUT,
    HTTP_HEAD,
    HTTP_PATCH
};

std::string GetNameForHttpMethod(const HttpMethod& httpMethod);

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
