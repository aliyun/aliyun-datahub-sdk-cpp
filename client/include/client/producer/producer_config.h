#ifndef INCLUDE_DATAHUB_CLIENT_PRODUCER_CONFIG_H
#define INCLUDE_DATAHUB_CLIENT_PRODUCER_CONFIG_H

#include "client/common_config.h"

namespace aliyun
{
namespace datahub
{

class ProducerConfiguration : public CommonConfiguration
{
public:
    ProducerConfiguration(const Account& account, const std::string& endpoint);
    virtual ~ProducerConfiguration() {}

    void SetMaxAsyncBufferRecords(int64_t value) { mMaxAsyncBufferRecords = value; }
    void SetMaxAsyncBufferSize(int64_t value) { mMaxAsyncBufferSize = value; }
    void SetMaxAsyncBufferTimeMs(int64_t value) { mMaxAsyncBufferTimeMs = value; }
    void SetMaxRecordPackQueueLimit(int32_t value) { mMaxRecordPackQueueLimit = value; }

    int64_t GetMaxAsyncBufferRecords() const { return mMaxAsyncBufferRecords; }
    int64_t GetMaxAsyncBufferSize() const { return mMaxAsyncBufferSize; }
    int64_t GetMaxAsyncBufferTimeMs() const { return mMaxAsyncBufferTimeMs; }
    int32_t GetMaxRecordPackQueueLimit() const { return mMaxRecordPackQueueLimit; }

private:
    int64_t mMaxAsyncBufferRecords;
    int64_t mMaxAsyncBufferSize;
    int64_t mMaxAsyncBufferTimeMs;
    int32_t mMaxRecordPackQueueLimit;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_PRODUCER_CONFIG_H