#include "datahub/datahub_error.h"

namespace aliyun
{
namespace datahub
{

const std::string ErrorCode::INVALID_PARAMETER = "InvalidParameter";
const std::string ErrorCode::INVALID_SUBSCRIPTION = "InvalidSubscription";
const std::string ErrorCode::INVALID_CURSOR = "InvalidCursor";

const std::string ErrorCode::RESOURCE_NOT_FOUND = "ResourceNotFound";
const std::string ErrorCode::NO_SUCH_TOPIC = "NoSuchTopic";
const std::string ErrorCode::NO_SUCH_PROJECT = "NoSuchProject";
const std::string ErrorCode::NO_SUCH_SUBSCRIPTION = "NoSuchSubscription";
const std::string ErrorCode::NO_SUCH_SHARD = "NoSuchShard";
const std::string ErrorCode::NO_SUCH_CONNECTOR = "NoSuchConnector";
const std::string ErrorCode::NO_SUCH_METER_INFO = "NoSuchMeteringInfo";
const std::string ErrorCode::NO_SUCH_ALARM_RULE = "NoSuchAlarmRule";

const std::string ErrorCode::RESOURCE_ALREADY_EXIST = "ResourceAlreadyExist";
const std::string ErrorCode::PROJECT_ALREADY_EXIST = "ProjectAlreadyExist";
const std::string ErrorCode::TOPIC_ALREADY_EXIST = "TopicAlreadyExist";
const std::string ErrorCode::CONNECTOR_ALREADY_EXIST = "ConnectorAlreadyExist";
const std::string ErrorCode::USER_ALREADY_EXIST = "UserAlreadyExist";
const std::string ErrorCode::ALARM_RULE_ALREADY_EXIST = "AlarmRuleAlreadyExist";

const std::string ErrorCode::RESOURCE_LIMIT = "ResourceLimit";
const std::string ErrorCode::SEEK_OUT_OF_RANGE = "SeekOutOfRange";
const std::string ErrorCode::UN_AUTHORIZED = "Unauthorized";
const std::string ErrorCode::NO_PERMISSION = "NoPermission";
const std::string ErrorCode::INVALID_SHARD_OPERATION = "InvalidShardOperation";
const std::string ErrorCode::OPERATOR_DENIED = "OperationDenied";
const std::string ErrorCode::LIMIT_EXCEED = "LimitExceeded";
const std::string ErrorCode::ODPS_SERVICE_ERROR = "OdpsServiceError";
const std::string ErrorCode::MYSQL_SERVICE_ERROR = "MysqlServiceError";
const std::string ErrorCode::INTERNAL_SERVER_ERROR = "InternalServerError";
const std::string ErrorCode::SUBSCRIPTION_OFFLINE = "SubscriptionOffline";
const std::string ErrorCode::OFFSET_RESETED = "OffsetReseted";
const std::string ErrorCode::OFFSET_SESSION_CLOSED = "OffsetSessionClosed";
const std::string ErrorCode::OFFSET_SESSION_CHANGED = "OffsetSessionChanged";
const std::string ErrorCode::MALFORMED_RECORD = "MalformedRecord";
const std::string ErrorCode::NO_SUCH_CONSUMER = "NoSuchConsumer";
const std::string ErrorCode::CONSUMER_GROUP_IN_PROCESS = "ConsumerGroupInProcess";
const std::string ErrorCode::EXPIRED_ACCESS_TOKEN = "ExpiredAccessToken";
const std::string ErrorCode::TOPIC_OFFLINE = "TopicOffline";

const std::set<std::string> ErrorCode::FATAL_ERROR_CODE{
    ErrorCode::NO_SUCH_PROJECT,
    ErrorCode::NO_SUCH_TOPIC,
    ErrorCode::NO_SUCH_SUBSCRIPTION,
    ErrorCode::NO_SUCH_CONNECTOR,
    ErrorCode::NO_SUCH_METER_INFO,
    ErrorCode::NO_SUCH_ALARM_RULE,
    ErrorCode::RESOURCE_ALREADY_EXIST,
    ErrorCode::PROJECT_ALREADY_EXIST,
    ErrorCode::TOPIC_ALREADY_EXIST,
    ErrorCode::CONNECTOR_ALREADY_EXIST,
    ErrorCode::USER_ALREADY_EXIST,
    ErrorCode::ALARM_RULE_ALREADY_EXIST,

    ErrorCode::UN_AUTHORIZED,
    ErrorCode::NO_PERMISSION,
    ErrorCode::OPERATOR_DENIED,
    ErrorCode::INVALID_SHARD_OPERATION,

    ErrorCode::SEEK_OUT_OF_RANGE,
    ErrorCode::MALFORMED_RECORD,
    ErrorCode::INVALID_PARAMETER,
    ErrorCode::INVALID_CURSOR,
    ErrorCode::RESOURCE_LIMIT,

    ErrorCode::OFFSET_RESETED,
    ErrorCode::OFFSET_SESSION_CHANGED,
    ErrorCode::OFFSET_SESSION_CLOSED,
};

bool ErrorCode::canRetry(const std::string& errorCode)
{
    return FATAL_ERROR_CODE.count(errorCode) == 0;
}

} // namespace datahub
} // namespace aliyun
