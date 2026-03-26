#ifndef DATAHUB_SDK_HTTP_REQUEST_H
#define DATAHUB_SDK_HTTP_REQUEST_H

#include <string>
#include <map>
#include <memory>
#include "http/uri.h"
#include "http/http_headers.h"
#include "http/http_types.h"

namespace aliyun
{
namespace datahub
{
namespace http
{

class HttpRequest
{
public:
    HttpRequest(const URI& uri, HttpMethod method) :
        mUri(uri), mMethod(method)
    {}

    virtual ~HttpRequest() {}

    virtual const std::map<std::string, std::string>& GetHeaders() const
    {
        return mHeaders;
    }

    virtual const std::string& GetHeaderValue(const std::string& headerName) const
    {
        return mHeaders.find(headerName)->second;
    }

    virtual void SetHeaderValue(const std::string& headerName, const std::string& headerValue)
    {
        mHeaders[headerName] = headerValue;
    }

    virtual void DeleteHeader(const std::string& headerName)
    {
    }

    virtual void AddContentBody(const std::string& strContent)
    {
        mContentBody = strContent;
    }

    virtual const std::string& GetContentBody() const
    {
        return mContentBody;
    }

    virtual bool HasHeader(const std::string& name) const
    {
        return true;
    }

    inline URI& GetUri()
    {
        return mUri;
    }

    const URI& GetUri() const { return mUri; }

    inline std::string GetURIString() const
    {
        return mUri.GetURIString();
    }

    inline const HttpMethod& GetMethod() const
    {
        return mMethod;
    }

    inline const std::string& GetDate() const
    {
        return GetHeaderValue(DATE_HEADER);
    }

    inline void SetDate(const std::string& value)
    {
        SetHeaderValue(DATE_HEADER, value);
    }

    inline const std::string& GetAuthorization() const
    {
        return GetHeaderValue(AUTHORIZATION_HEADER);
    }

    inline void SetAuthorization(const std::string& value)
    {
        SetHeaderValue(AUTHORIZATION_HEADER, value);
    }

    inline const std::string& GetSecurityToken() const
    {
        return GetHeaderValue(SECURITY_TOKEN_HEADER);
    }

    inline void SetSecurityToken(const std::string& token)
    {
        SetHeaderValue(SECURITY_TOKEN_HEADER, token);
    }

    inline const std::string& GetContentLength() const
    {
        return GetHeaderValue(CONTENT_LENGTH_HEADER);
    }

    inline void SetContentLength(const std::string& value)
    {
        SetHeaderValue(CONTENT_LENGTH_HEADER, value);
    }

    inline const std::string& GetContentType() const
    {
        return GetHeaderValue(CONTENT_TYPE_HEADER);
    }

    inline void SetContentType(const std::string& value)
    {
        SetHeaderValue(CONTENT_TYPE_HEADER, value);
    }

    inline const std::string& GetUserAgent() const
    {
        return GetHeaderValue(USER_AGENT_HEADER);
    }

    inline void SetUserAgent(const std::string& value)
    {
        SetHeaderValue(USER_AGENT_HEADER, value);
    }

    inline void SetClientVersion(const std::string& value)
    {
        SetHeaderValue(CLIENT_VERSION_HEADER, value);
    }

private:
    URI mUri;
    HttpMethod mMethod;
    std::map<std::string, std::string> mHeaders;
    std::string mContentBody;
};

typedef std::shared_ptr<HttpRequest> HttpRequestPtr;

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
