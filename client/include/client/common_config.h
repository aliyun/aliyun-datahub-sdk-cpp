#ifndef INCLUDE_DATAHUB_CLIENT_COMMON_CONFIG_H
#define INCLUDE_DATAHUB_CLIENT_COMMON_CONFIG_H

#include "datahub/datahub_config.h"

namespace aliyun
{
namespace datahub
{

class CommonConfiguration : public Configuration
{
public:
    CommonConfiguration(const Account& account, const std::string& endpoint);
    virtual ~CommonConfiguration() {}

    int32_t GetAsycFetchThreadNum() const { return mAsycFetchThreadNum; }
    int32_t GetQueueLimitNum() const { return mQueueLimitNum; }
    int32_t GetRetryTimes() const { return mRetryTimes; }
    const std::string& GetLogPriorityLevel() const { return mLogPriorityLevel; }
    std::string GetLogFilePath() const { return mLogFilePath; }

    void SetAsycFetchThreadNum(int32_t value) { mAsycFetchThreadNum = value; }
    void SetQueueLimitNum(int32_t value) { mQueueLimitNum = value; }
    void SetRetryTimes(int32_t value) { mRetryTimes = value; }
    void SetLogPriorityLevel(const std::string& value) { mLogPriorityLevel = value; };
    void SetLogFilePath(const std::string& value) { mLogFilePath = value; }

protected:
    int32_t mAsycFetchThreadNum;
    int32_t mQueueLimitNum;
    int32_t mRetryTimes;
    std::string mLogPriorityLevel;
    std::string mLogFilePath;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_COMMON_CONFIG_H