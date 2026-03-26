#ifndef INCLUDE_DATAHUB_CLIENT_CONSUMER_CONFIG_H
#define INCLUDE_DATAHUB_CLIENT_CONSUMER_CONFIG_H

#include "client/common_config.h"


namespace aliyun
{
namespace datahub
{

class ConsumerConfiguration : public CommonConfiguration
{
public:

    ConsumerConfiguration(const Account& account, const std::string& endpoint);
    virtual ~ConsumerConfiguration() {}

    void SetAutoAckOffset(bool value) { mAutoAckOffset = value; }
    void SetFetchLimitNum(int32_t value) { mFetchLimitNum = value; }
    void SetSessionTimeout(int64_t value) { mSessionTimeout = value; }
    void SetRecordFetchQueueLimit(uint32_t value) { mRecordFetchQueueLimit = value; }

    bool GetAutoAckOffset() const { return mAutoAckOffset; }
    int32_t GetFetchLimitNum() const { return mFetchLimitNum; }
    int64_t GetSessionTimeout() const { return mSessionTimeout; }
    uint32_t GetRecordFetchQueueLimit() const { return mRecordFetchQueueLimit; }

private:
    bool mAutoAckOffset;
    int32_t mFetchLimitNum;
    int64_t mSessionTimeout;
    uint32_t mRecordFetchQueueLimit;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_CONSUMER_CONFIG_H