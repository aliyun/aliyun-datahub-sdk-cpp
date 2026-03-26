#ifndef DATAHUB_SDK_ERROR_PARSER_H
#define DATAHUB_SDK_ERROR_PARSER_H

#include <string>
#include "datahub/datahub_exception.h"
#include "http/http_response.h"

namespace aliyun
{
namespace datahub
{

class ErrorParser
{
public:
    ErrorParser() {};
    ~ErrorParser() {};

    static DatahubException Parse(const http::HttpResponse& response);
};

} // namespace datahub
} // namespace aliyun
#endif
