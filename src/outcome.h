#ifndef DATAHUB_SDK_OUTCOME_H
#define DATAHUB_SDK_OUTCOME_H

#include <string>
#include <memory>
#include <http/http_response.h>

namespace aliyun
{
namespace datahub
{

class Outcome
{
public:
    Outcome(std::shared_ptr<http::HttpResponse> response) :
        mSuccess(true),
        mError(),
        mHttpResponse(response)
    {
    }

    Outcome(bool success, const std::string& error, std::shared_ptr<http::HttpResponse> response) :
        mSuccess(success),
        mError(error),
        mHttpResponse(response)
    {
    }

    bool IsSuccess() const
    {
        return mSuccess;
    }

    std::string GetError() const
    {
        return mError;
    }

    std::shared_ptr<http::HttpResponse> GetHttpResponse() const
    {
        return mHttpResponse;
    }

private:
    bool mSuccess;
    std::string mError;
    std::shared_ptr<http::HttpResponse> mHttpResponse;

};


} // namespace datahub
} // namespace aliyun
#endif
