#include <string>
#include "rapidjson/document.h"
#include "http/http_response.h"
#include "error_parser.h"

namespace aliyun
{
namespace datahub
{

DatahubException ErrorParser::Parse(const http::HttpResponse& response)
{
    std::string errorCode, errorMessage, errorDetail;
    const std::string& requestId = response.GetHeader(http::REQUEST_ID_HEADER);
    if (response.GetHeader("Content-Type") != http::CONTENT_TYPE_JSON)
    {
        errorCode = std::to_string((int)response.GetResponseCode());
        errorMessage = *response.GetResponseBody();
        return DatahubException(response.GetResponseCode(), errorCode, errorMessage, requestId);
    }

    const std::string& body = *response.GetResponseBody();
    rapidjson::Document doc;
    if (doc.Parse<rapidjson::kParseValidateEncodingFlag | rapidjson::kParseStopWhenDoneFlag>(body.c_str()).HasParseError())
    {
        std::string error = "Parse response failed: ";
        error.append(std::to_string(doc.GetParseError())).append(" Offset: ");
        error.append(std::to_string(doc.GetErrorOffset())).append(" Body: ").append(body);
        return DatahubException(response.GetResponseCode(), "Parse response failed", error, requestId);
    }

    rapidjson::Value::ConstMemberIterator it = doc.FindMember("ErrorCode");
    if (it != doc.MemberEnd() && it->value.IsString())
    {
        errorCode.assign(it->value.GetString(), it->value.GetStringLength());
    }

    it = doc.FindMember("ErrorMessage");
    if (it != doc.MemberEnd() && it->value.IsString())
    {
        errorMessage.assign(it->value.GetString(), it->value.GetStringLength());
    }

    it = doc.FindMember("ErrorDetail");
    if (it != doc.MemberEnd() && it->value.IsString())
    {
        errorDetail.assign(it->value.GetString(), it->value.GetStringLength());
    }

    if (errorDetail.empty())
    {
        return DatahubException(response.GetResponseCode(), errorCode, errorMessage, requestId);
    }
    else
    {
        return DatahubException(response.GetResponseCode(), errorCode, errorMessage, errorDetail, requestId);
    }
}

} // namespace datahub
} // namespace aliyun
