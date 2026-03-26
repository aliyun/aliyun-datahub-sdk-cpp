#ifndef DATAHUB_SDK_DATAHUB_ERROR_H
#define DATAHUB_SDK_DATAHUB_ERROR_H

#include <set>
#include <string>

namespace aliyun
{
namespace datahub
{

class ErrorCode
{
public:
    const static std::string INVALID_PARAMETER;
    const static std::string INVALID_SUBSCRIPTION;
    const static std::string INVALID_CURSOR;

    const static std::string RESOURCE_NOT_FOUND;
    const static std::string NO_SUCH_TOPIC;
    const static std::string NO_SUCH_PROJECT;
    const static std::string NO_SUCH_SUBSCRIPTION;
    const static std::string NO_SUCH_SHARD;
    const static std::string NO_SUCH_CONNECTOR;
    const static std::string NO_SUCH_METER_INFO;
    const static std::string NO_SUCH_ALARM_RULE;

    const static std::string RESOURCE_ALREADY_EXIST;
    const static std::string PROJECT_ALREADY_EXIST;
    const static std::string TOPIC_ALREADY_EXIST;
    const static std::string CONNECTOR_ALREADY_EXIST;
    const static std::string USER_ALREADY_EXIST;
    const static std::string ALARM_RULE_ALREADY_EXIST;

    const static std::string RESOURCE_LIMIT;
    const static std::string SEEK_OUT_OF_RANGE;
    const static std::string UN_AUTHORIZED;
    const static std::string NO_PERMISSION;
    const static std::string INVALID_SHARD_OPERATION;
    const static std::string OPERATOR_DENIED;
    const static std::string LIMIT_EXCEED;
    const static std::string ODPS_SERVICE_ERROR;
    const static std::string MYSQL_SERVICE_ERROR;
    const static std::string INTERNAL_SERVER_ERROR;
    const static std::string SUBSCRIPTION_OFFLINE;
    const static std::string OFFSET_RESETED;
    const static std::string OFFSET_SESSION_CLOSED;
    const static std::string OFFSET_SESSION_CHANGED;
    const static std::string MALFORMED_RECORD;
    const static std::string NO_SUCH_CONSUMER;
    const static std::string CONSUMER_GROUP_IN_PROCESS;
    const static std::string EXPIRED_ACCESS_TOKEN;
    const static std::string TOPIC_OFFLINE;

    const static std::set<std::string> FATAL_ERROR_CODE;

    static bool canRetry(const std::string& errorCode);
};

} // namespace datahub
} // namespace aliyun

#endif // DATAHUB_SDK_DATAHUB_ERROR_H