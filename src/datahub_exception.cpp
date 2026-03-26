#include <string>
#include <exception>
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

const char* LOCAL_ERROR_CODE = "LocalError";
const char* SERVICE_ERROR_CODE = "InternalServiceError";

DatahubException::DatahubException(const std::string& errorCode,
        const std::string& errorMessage, const std::string& requestId)
    :mHttpCode(0),
     mErrorCode(errorCode),
     mErrorMessage(errorMessage),
     mRequestId(requestId),
     mMessage(BuildMessage())
{
}

DatahubException::DatahubException(uint32_t httpCode, const std::string& errorCode,
        const std::string& errorMessage, const std::string& requestId)
    :mHttpCode(httpCode),
     mErrorCode(errorCode),
     mErrorMessage(errorMessage),
     mRequestId(requestId),
     mMessage(BuildMessage())
{
}

DatahubException::DatahubException(uint32_t httpCode, const std::string& errorCode,
        const std::string& errorMessage, const std::string& errorDetail, const std::string& requestId)
    :mHttpCode(httpCode),
     mErrorCode(errorCode),
     mErrorMessage(errorMessage),
     mErrorDetail(errorDetail),
     mRequestId(requestId),
     mMessage(BuildMessage())
{
}

const char* DatahubException::what() const throw()
{
    return  mMessage.c_str();
}

std::string DatahubException::BuildMessage() const
{
    std::string message;
    if (!mRequestId.empty())
    {
        message.append("RequestId=").append(mRequestId);
    }

    message.append(", ErrorCode=").append(mErrorCode);
    message.append(", ErrorMessage=").append(mErrorMessage);
    if (!mErrorDetail.empty())
    {
        message.append(", ErrorDetail=").append(mErrorDetail);
    }

    return message;
}

} // namespace datahub
} // namespace aliyun
