#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

std::string GetNameForFieldType(const FieldType& fieldType)
{
    switch (fieldType)
    {
        case BIGINT:
            return "BIGINT";
        case DOUBLE:
            return "DOUBLE";
        case BOOLEAN:
            return "BOOLEAN";
        case TIMESTAMP:
            return "TIMESTAMP";
        case STRING:
            return "STRING";
        case DECIMAL:
            return "DECIMAL";
        case INTEGER:
            return "INTEGER";
        case FLOAT:
            return "FLOAT";
        case TINYINT:
            return "TINYINT";
        case SMALLINT:
            return "SMALLINT";
        case JSON:
            return "JSON";
        default:
            throw DatahubException(LOCAL_ERROR_CODE, "invalid field type");
    }
}

FieldType GetFieldTypeForName(const std::string& name)
{
    if (name == "BIGINT")
    {
        return BIGINT;
    }
    else if (name == "DOUBLE")
    {
        return DOUBLE;
    }
    else if (name == "BOOLEAN")
    {
        return BOOLEAN;
    }
    else if (name == "TIMESTAMP")
    {
        return TIMESTAMP;
    }
    else if (name == "STRING")
    {
        return STRING;
    }
    else if (name == "DECIMAL")
    {
        return DECIMAL;
    }
    else if (name == "INTEGER")
    {
        return INTEGER;
    }
    else if (name == "FLOAT")
    {
        return FLOAT;
    }
    else if (name == "TINYINT")
    {
        return TINYINT;
    }
    else if (name == "SMALLINT")
    {
        return SMALLINT;
    }
    else if (name == "JSON")
    {
        return JSON;
    }
    else
    {
        throw DatahubException(LOCAL_ERROR_CODE, "unsupported field type " + name);
    }
}

} // namespace datahub
} // namespace aliyun
