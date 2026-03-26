#include "timer.h"
#include "datahub/datahub_exception.h"
#include <sys/timeb.h>

namespace aliyun
{
namespace datahub
{

Timer::Timer(int64_t timeout)
{
    Reset(timeout);
    mMutexPtr.reset(new std::mutex());
    mConditionVarPtr.reset(new std::condition_variable());
}

void Timer::Reset(int64_t timeout)
{
    mTimeout = timeout;
    mStartTimeout = GetCurrentTimeMs();
    if (timeout < INT64_MAX - mStartTimeout)
    {
        mDeadlineTimeout = mStartTimeout + mTimeout;
    }
    else
    {
        mDeadlineTimeout = INT64_MAX;
    }
}

void Timer::Reset()
{
    Reset(mTimeout);
}

void Timer::ResetDeadline()
{
    mDeadlineTimeout = 0;
}

bool Timer::IsExpired(int64_t timeout)
{
    return timeout >= mDeadlineTimeout;
}

bool Timer::IsExpired()
{
    return IsExpired(GetCurrentTimeMs());
}

int64_t Timer::Elapsed()
{
    return GetCurrentTimeMs() - mStartTimeout;
}

void Timer::WaitExpire()
{
    WaitExpire(INT64_MAX);
}

void Timer::WaitExpire(int64_t timeout)
{
    int64_t tmp = mDeadlineTimeout-GetCurrentTimeMs();
    if (tmp < timeout)
    {
        timeout = tmp;
    }
    if (timeout > 0)
    {
        std::unique_lock<std::mutex> ul(*mMutexPtr);
        std::chrono::milliseconds ms(timeout);
        mConditionVarPtr->wait_for(ul, ms);
    }
}

void Timer::NotifyAll()
{
    mConditionVarPtr->notify_all();
}

void Timer::NotifyOne()
{
    mConditionVarPtr->notify_one();
}

bool Timer::WaitFor(std::mutex& mutex, std::condition_variable& cv, std::function<bool()>& func)
{
    while(true)
    {
        if (func())
        {
            return true;
        }

        int64_t curr = GetCurrentTimeMs();
        if (curr >= mDeadlineTimeout)
        {
            return false;
        }

        std::unique_lock<std::mutex> ul(mutex);
        std::chrono::milliseconds ms(mDeadlineTimeout-curr);
        cv.wait_for(ul, ms);
    }
}

int64_t Timer::GetCurrentTimeMs()
{
    timeb t;
    ftime(&t);
    return t.time*1000 + t.millitm;
}

} // namespace datahub
} // namespace aliyun
