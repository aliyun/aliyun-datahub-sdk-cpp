#ifndef INCLUDE_DATAHUB_CLIENT_TIMER_H
#define INCLUDE_DATAHUB_CLIENT_TIMER_H

#include <mutex>
#include <memory>
#include <condition_variable>
#include <functional>

namespace aliyun
{
namespace datahub
{

class Timer
{
public:
    Timer(int64_t timeout = 0l);

    void Reset();
    void Reset(int64_t timeout);
    void ResetDeadline();
    bool IsExpired();
    bool IsExpired(int64_t timeout);
    int64_t Elapsed();
    void WaitExpire();
    void WaitExpire(int64_t timeout);
    void NotifyAll();
    void NotifyOne();
    bool WaitFor(std::mutex& lock, std::condition_variable& cv, std::function<bool()>& func);

    static int64_t GetCurrentTimeMs();
    int64_t GetTimeout() const { return mTimeout; };
    int64_t GetStartTimeout() const { return mStartTimeout; };
    int64_t GetDeadlineTimeout() const { return mDeadlineTimeout; };
    int64_t GetRemainTimeMs() const { return mDeadlineTimeout - GetCurrentTimeMs(); }

private:
    int64_t mTimeout;
    int64_t mStartTimeout;
    int64_t mDeadlineTimeout;

    std::unique_ptr<std::condition_variable> mConditionVarPtr;
    std::unique_ptr<std::mutex> mMutexPtr;
};

typedef std::shared_ptr<Timer> TimerPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_TIMER_H
