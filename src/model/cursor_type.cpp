#include <string>
#include "datahub/datahub_request.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

std::string GetNameForCursorType(const CursorType& cursorType)
{
    switch (cursorType)
    {
        case CURSOR_TYPE_OLDEST:
            return "OLDEST";
        case CURSOR_TYPE_LATEST:
            return "LATEST";
        case CURSOR_TYPE_TIMESTAMP:
            return "SYSTEM_TIME";
        case CURSOR_TYPE_SEQUENCE:
            return "SEQUENCE";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid cursor type");
    }
}

} // namespace datahub
} // namespace aliyun
