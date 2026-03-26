#ifndef DATAHUB_SDK_DATAHUB_EXCEPTION_H
#define DATAHUB_SDK_DATAHUB_EXCEPTION_H

#include <string>
#include <exception>

namespace aliyun
{
namespace datahub
{

extern const char* LOCAL_ERROR_CODE;
extern const char* SERVICE_ERROR_CODE;

class DatahubException : public std::exception
{
public:
    DatahubException(const std::string& errorCode, const std::string& errorMessage, const std::string& requestId = "");
    DatahubException(uint32_t httpCode, const std::string& errorCode, const std::string& errorMessage, const std::string& requestId = "");
    DatahubException(uint32_t httpCode, const std::string& errorCode, const std::string& errorMessage, const std::string& errorDetail, const std::string& requestId);

    ~DatahubException() throw() {}

    std::string GetErrorCode() const { return mErrorCode; }
    std::string GetErrorMessage() const { return mErrorMessage; }
    std::string GetErrorDetail() const { return mErrorDetail; }
    std::string GetRequestId() const { return mRequestId; }
    std::string GetMessage() const { return mMessage; }
    uint32_t GetHttpCode() const { return mHttpCode; }
    virtual const char* what() const throw();
    std::string BuildMessage() const;

private:
    uint32_t    mHttpCode;
    std::string mErrorCode;
    std::string mErrorMessage;
    std::string mErrorDetail;
    std::string mRequestId;
    std::string mMessage;
};

} // namespace datahub
} // namespace aliyun
#endif
