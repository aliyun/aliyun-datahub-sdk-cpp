#ifndef INCLUDE_DATAHUB_CLIENT_WRITE_RESULT_H
#define INCLUDE_DATAHUB_CLIENT_WRITE_RESULT_H

#include <stdint.h>
#include <string>
#include <memory>
#include <future>

namespace aliyun
{
namespace datahub
{

class WriteResult
{
public:
    WriteResult(const std::string& shardId, int64_t elapsedTimeMs, int64_t sendTimeMs)
        : mShardId(shardId),
          mElapsedTimeMs(elapsedTimeMs),
          mSendTimeMs(sendTimeMs)
    {}

    std::string GetShardId() const { return mShardId; }
    int64_t GetElapsedTimeMs() const { return mElapsedTimeMs; }
    int64_t GetSendTimeMs() const { return mSendTimeMs; }

private:
    std::string mShardId;
    int64_t mElapsedTimeMs;
    int64_t mSendTimeMs;
};

typedef std::shared_ptr<WriteResult> WriteResultPtr;
typedef std::shared_future<WriteResultPtr> WriteResultFuture;
typedef std::shared_ptr<WriteResultFuture> WriteResultFuturePtr;
typedef std::shared_ptr<std::promise<WriteResultPtr> > WriteResultPromisePtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_WRITE_RESULT_H