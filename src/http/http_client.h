#ifndef DATAHUB_SDK_HTTP_CLIENT_H
#define DATAHUB_SDK_HTTP_CLIENT_H

#include <memory>
#include "outcome.h"
#include "http/http_request.h"
#include "http/http_response.h"

namespace aliyun
{
namespace datahub
{
namespace http
{

class HttpRequest;
class HttpResponse;

/**
  * Abstract HttpClient. All it does is make HttpRequests and return their response.
  */
class HttpClient
{
public:
    HttpClient() {};
    virtual ~HttpClient() {}

    /*
    * Takes an http request, makes it, and returns the newly allocated HttpResponse
    */
    virtual Outcome MakeRequest(HttpRequest& request) const = 0;
};

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
