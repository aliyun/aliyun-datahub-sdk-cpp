#ifndef DATAHUB_SDK_AUTH_SIGNER_H
#define DATAHUB_SDK_AUTH_SIGNER_H

#include <map>
#include <string>
#include <memory>
#include "http/http_request.h"

namespace aliyun
{
namespace datahub
{

namespace http
{
    class HttpRequest;
}

class AuthSigner
{
public:
    AuthSigner(const std::string& accessId, const std::string& accessKey, const std::string& region);
    AuthSigner(const std::string& accessId, const std::string& accessKey, const std::string& token, const std::string& region);
    ~AuthSigner();

    void SignRequest(http::HttpRequest& request, bool v4 = false) const;

private:
    static std::string Sha1Hmac(const std::string& key, const std::string& data);
    static std::string Sha256Hmac(const std::string& key, const std::string& data);
    static std::string Base64Encode(const std::string& data);
    static std::string GetSigningKey(const std::string& secret, const std::string& date, const std::string& region, const std::string& product);
    static std::string BuildHeaderString(const std::map<std::string, std::string>& headers, const std::string& prefix);
    static std::string BuildCanonicalString(const http::HttpRequest& request, const std::string& prefix);

private:
    std::string mAccessId;
    std::string mAccessKey;
    std::string mSecurityToken;
    std::string mRegion;
};

} // namespace datahub
} // namespace aliyun
#endif
