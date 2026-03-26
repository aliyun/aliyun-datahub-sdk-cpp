#include "thread_pool.h"
#include "datahub/datahub_exception.h"


namespace aliyun
{
namespace datahub
{

/* ThreadPool */

ThreadPool::ThreadPool(int32_t threadNum, int32_t queueLimit, const std::string& name)
    : mClosed(true),
      mQueueLimit(queueLimit),
      mPoolName(name)
{
    mThreadNum = std::max(threadNum, 1);
}

ThreadPool::~ThreadPool()
{
    Shutdown();
}

bool ThreadPool::IsRunning()
{
    return !mClosed.load();
}

bool ThreadPool::Start()
{
    bool expectStatu = true;
    if (!mClosed.compare_exchange_strong(expectStatu, false))
    {
        return false;
    }

    for (int32_t i = 0; i < mThreadNum; i++)
    {
        mThreads.emplace_back(
            [this]{
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mQueueMutex);
                        mCondition.wait(lock, [this]{return mClosed.load() || !mTaskQueue.empty();});
                        if (mClosed.load() && mTaskQueue.empty())
                        {
                            return ;
                        }
                        task = std::move(mTaskQueue.front());
                        mTaskQueue.pop();
                    }
                    task();
                }
            }
        );
    }
    return true;
}

bool ThreadPool::Shutdown()
{
    bool expectStatu = false;
    if (!mClosed.compare_exchange_strong(expectStatu, true))
    {
        return false;
    }

    mCondition.notify_all();
    for (std::thread& thread : mThreads)
    {
        thread.join();
    }
    return true;
}

bool ThreadPool::Submit(std::function<void()> func)
{
    if (mClosed.load())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Submit on closed thread pool. name: " + mPoolName);
    }

    std::unique_lock<std::mutex> lock(mQueueMutex);
    if (mTaskQueue.size() < mQueueLimit)
    {
        mTaskQueue.emplace(func);
        mCondition.notify_all();
        return true;
    }
    else
    {
        return false;
    }
}


/* HashThreadPool */

HashThreadPool::HashThreadPool(int32_t threadNum, int32_t queueLimit, const std::string& name)
    : mClosed(true),
      mQueueLimit(queueLimit),
      mPoolName(name)
{
    mThreadNum = std::max(threadNum, 1);
}

HashThreadPool::~HashThreadPool()
{
    Shutdown();
}

bool HashThreadPool::IsRunning()
{
    return !mClosed.load();
}

bool HashThreadPool::Start()
{
    bool expectStatu = true;
    if (!mClosed.compare_exchange_strong(expectStatu, false))
    {
        return false;
    }

    for (int32_t i = 0; i < mThreadNum; i++)
    {
        ThreadPoolPtr threadPool = std::make_shared<ThreadPool>(1u, mQueueLimit, "single_" + std::to_string(i));
        threadPool->Start();
        mWorkers.push_back(threadPool);
    }
    return true;
}

bool HashThreadPool::Shutdown()
{
    bool expectStatu = false;
    if (!mClosed.compare_exchange_strong(expectStatu, true))
    {
        return false;
    }

    for (auto it = mWorkers.begin(); it != mWorkers.end(); it++)
    {
        (*it)->Shutdown();
    }
    return true;
}

bool HashThreadPool::Submit(int32_t key, std::function<void()> func)
{
    if (mClosed.load())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Submit on closed hash thread pool. name: " + mPoolName);
    }

    uint32_t index = key % mThreadNum;
    const ThreadPoolPtr& threadInfo = mWorkers[index];
    return threadInfo->Submit(func);
}

} // namespace datahub
} // namespace aliyun
