#include "auth_signer.h"
#include "utils.h"
#include <map>
#include <string>
#include "openssl/hmac.h"
#include "openssl/bio.h"
#include "openssl/evp.h"
#include "openssl/buffer.h"

namespace aliyun
{
namespace datahub
{

const std::string NEW_LINE = "\n";
const std::string V4_PREFIX = "aliyun_v4";
const std::string V4_REQUEST = "aliyun_v4_request";
const std::string V4_PRODUCT = "datahub";

AuthSigner::AuthSigner(const std::string& accessId, const std::string& accessKey, const std::string& region) :
    mAccessId(accessId),
    mAccessKey(accessKey),
    mRegion(region)
{
}

AuthSigner::AuthSigner(const std::string& accessId, const std::string& accessKey, const std::string& token, const std::string& region) :
    mAccessId(accessId),
    mAccessKey(accessKey),
    mSecurityToken(token),
    mRegion(region)
{
}

AuthSigner::~AuthSigner()
{
}

std::string AuthSigner::Sha1Hmac(const std::string& key, const std::string& data)
{
    char buf[EVP_MAX_MD_SIZE];
    unsigned int size = 0;
    HMAC(EVP_sha1(), key.c_str(), key.size(), (unsigned char*)data.c_str(), data.size(), (unsigned char*)buf, &size);
    return std::string(buf, size);
}

std::string AuthSigner::Sha256Hmac(const std::string& key, const std::string& data)
{
    char buf[EVP_MAX_MD_SIZE];
    unsigned int size = 0;
    HMAC(EVP_sha256(), key.c_str(), key.size(), (unsigned char*)data.c_str(), data.size(), (unsigned char*)buf, &size);
    return std::string(buf, size);
}

std::string AuthSigner::GetSigningKey(const std::string& secret, const std::string& date, const std::string& region, const std::string& product)
{
    const std::string& first = Sha256Hmac(V4_PREFIX + secret, date);
    const std::string& second = Sha256Hmac(first, region);
    const std::string& third = Sha256Hmac(second, product);
    const std::string& signkey = Sha256Hmac(third, V4_REQUEST);
    return Base64Encode(signkey);
}

std::string AuthSigner::Base64Encode(const std::string& data)
{
    std::string result;
    result.resize(4 * (data.size()/3 + 1) + 1);
    int len = EVP_EncodeBlock((uint8_t*)(&result[0]), (const uint8_t*)data.c_str(), data.size());
    result.resize(len);
    return result;
}

std::string AuthSigner::BuildHeaderString(const std::map<std::string, std::string>& headers, const std::string& prefix)
{
    std::string headerString;
    headerString.reserve(1024);
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        if (it->first.find(prefix) == 0)
        {
            headerString.append(it->first).append(":").append(it->second);
            headerString.append(NEW_LINE);
        }
    }

    headerString.resize(std::max(1lu, headerString.size()) - 1);
    return headerString;
}

std::string AuthSigner::BuildCanonicalString(const http::HttpRequest& request, const std::string& prefix)
{
    const std::string& method = GetNameForHttpMethod(request.GetMethod());
    const std::string& date = request.GetDate();
    const std::string& contentType = request.GetContentType();
    const std::string& headerString = BuildHeaderString(request.GetHeaders(), prefix);
    const http::URI uri = request.GetUri();
    const std::string& resourcePath = uri.GetPath();

    std::string stringToSign;
    stringToSign.reserve(2048);

    stringToSign.append(method).append(NEW_LINE).append(contentType).append(NEW_LINE)
    .append(date).append(NEW_LINE).append(headerString).append(NEW_LINE).append(resourcePath);

    return stringToSign;
}

void AuthSigner::SignRequest(http::HttpRequest& request, bool v4) const
{
    if (!mSecurityToken.empty())
    {
        request.SetSecurityToken(mSecurityToken);
    }

    std::string authorization;
    const std::string& stringToSign = BuildCanonicalString(request, "x-datahub-");
    if (!v4)
    {
        const std::string& hmacString = Sha1Hmac(mAccessKey, stringToSign);
        const std::string& signature = Base64Encode(hmacString);
        authorization.append("DATAHUB ").append(mAccessId).append(":").append(signature);
    }
    else
    {
        const std::string& date = Utils::GetDayFromTimeStamp(time(NULL), true);
        const std::string& signKey = GetSigningKey(mAccessKey, date, mRegion, V4_PRODUCT);
        const std::string& hmacString = Sha256Hmac(signKey, stringToSign);
        const std::string& signature = Base64Encode(hmacString);
        authorization.append("ACS3-HMAC-SHA256 Credential=").append(mAccessId);
        authorization.append("/").append(date).append("/").append(mRegion).append("/datahub/aliyun_v4_request");
        authorization.append(",Signature=").append(signature);
    }

    request.SetAuthorization(authorization);
}

} // namespace datahub
} // namespace aliyun
