#ifndef DATAHUB_SDK_CURL_HTTP_CLIENT_H
#define DATAHUB_SDK_CURL_HTTP_CLIENT_H

#include "http/http_client.h"
#include "http/curl/curl_handle_container.h"

namespace aliyun
{
namespace datahub
{
namespace http
{

class CurlHttpClient: public HttpClient
{
public:
    CurlHttpClient(unsigned maxPoolSize, long requestTimeout, long connectTimeout);
    Outcome MakeRequest(HttpRequest& request) const;

private:
    mutable CurlHandleContainer mCurlHandleContainer;

    static size_t ReadBody(char* ptr, size_t size, size_t nmemb, void* userdata);

    static size_t WriteData(char* ptr, size_t size, size_t nmemb, void* userdata);

    static size_t WriteHeader(char* ptr, size_t size, size_t nmemb, void* userdata);
};

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
