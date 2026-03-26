#include "datahub/datahub_record.h"

namespace aliyun
{
namespace datahub
{

std::string GetNameForRecordType(const RecordType& recordType)
{
    switch (recordType)
    {
        case TUPLE:
            return "TUPLE";
        case BLOB:
            return "BLOB";
        default:
            return "TUPLE";
    }
}

} // namespace datahub
} // namespace aliyun
