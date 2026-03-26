#include "client/common_config.h"
#include "client/consumer/consumer_config.h"
#include "client/producer/producer_config.h"
#include "constant.h"

namespace aliyun
{
namespace datahub
{

CommonConfiguration::CommonConfiguration(const Account& account, const std::string& endpoint)
    : Configuration(account, endpoint)
    , mAsycFetchThreadNum(DEFAULT_FETCH_THREAD_NUMBER)
    , mQueueLimitNum(DEFAULT_QUEUE_LIMIT_NUMBER)
    , mRetryTimes(DEFAULT_RETRY_TIMES)
    , mLogPriorityLevel("INFO")
    , mLogFilePath("./DatahubClient.log")
{
    mUserInfo = DATAHUB_CLIENT;
}

ProducerConfiguration::ProducerConfiguration(const Account& account, const std::string& endpoint)
    : CommonConfiguration(account, endpoint)
    , mMaxAsyncBufferRecords(DEFAULT_MAX_ASYNC_BUFFER_RECOED_COUNT)
    , mMaxAsyncBufferSize(DEFAULT_MAX_ASYNC_BUFFER_SIZE)
    , mMaxAsyncBufferTimeMs(DEFAULT_MAX_ASYNC_BUFFER_TIME_MS)
    , mMaxRecordPackQueueLimit(DEFAULT_MAX_RECORD_PACK_QUEUE_LIMIT)
{}

ConsumerConfiguration::ConsumerConfiguration(const Account& account, const std::string& endpoint)
    : CommonConfiguration(account, endpoint)
    , mAutoAckOffset(true)
    , mFetchLimitNum(DEFAULT_LIMIT_NUM)
    , mSessionTimeout(MIN_CONSUMER_SESSION_TIMEOUT)
    , mRecordFetchQueueLimit(DEFAULT_RECORD_FETCH_QUEUE_LIMIT)
{}

} // namespace datahub
} // namespace aliyun
