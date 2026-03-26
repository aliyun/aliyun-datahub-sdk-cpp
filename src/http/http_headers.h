#ifndef DATAHUB_SDK_HTTP_HEADERS_H
#define DATAHUB_SDK_HTTP_HEADERS_H
#include <string>

namespace aliyun
{
namespace datahub
{
namespace http
{

const std::string DATE_HEADER = "Date";
const std::string AUTHORIZATION_HEADER = "Authorization";
const std::string CONTENT_LENGTH_HEADER = "Content-Length";
const std::string CONTENT_TYPE_HEADER = "Content-Type";
const std::string USER_AGENT_HEADER = "User-Agent";
const std::string CLIENT_VERSION_HEADER = "x-datahub-client-version";
const std::string SECURITY_TOKEN_HEADER = "x-datahub-security-token";
const std::string RAW_SIZE_HEADER = "x-datahub-content-raw-size";
const std::string REQUEST_ID_HEADER = "x-datahub-request-id";
const std::string CONTENT_ENCODING = "Content-Encoding";
const std::string ACCEPT_ENCODING = "Accept-Encoding";
const std::string REQUEST_ACTION = "x-datahub-request-action";
const std::string REQUEST_SUB_ID = "x-datahub-sub-id";

const std::string CONTENT_TYPE_JSON = "application/json";
const std::string CONTENT_TYPE_PROTOBUF = "application/x-protobuf";

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
