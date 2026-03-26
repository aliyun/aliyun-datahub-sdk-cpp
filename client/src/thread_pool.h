#ifndef INCLUDE_DATAHUB_CLIENT_THREAD_POOL_H
#define INCLUDE_DATAHUB_CLIENT_THREAD_POOL_H

#include <queue>
#include <atomic>
#include <string>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace aliyun
{
namespace datahub
{

class ThreadPool
{
public:
    ThreadPool(int32_t threadNum, int32_t queueLimit, const std::string& name);
    ~ThreadPool();

    bool Start();
    bool Shutdown();

    bool IsRunning();
    bool Submit(std::function<void()> func);

private:
    std::atomic<bool> mClosed;
    int32_t mThreadNum;
    uint32_t mQueueLimit;
    std::string mPoolName;
    std::mutex mQueueMutex;
    std::condition_variable mCondition;
    std::vector<std::thread> mThreads;
    std::queue<std::function<void()> > mTaskQueue;
};

typedef std::shared_ptr<ThreadPool> ThreadPoolPtr;


class HashThreadPool
{
public:
    HashThreadPool(int32_t threadNum, int32_t queueLimit, const std::string& name);
    ~HashThreadPool();

    bool Start();
    bool Shutdown();

    bool IsRunning();
    bool Submit(int32_t key, std::function<void()> func);

private:
    std::atomic<bool> mClosed;
    int32_t mThreadNum;
    uint32_t mQueueLimit;
    std::string mPoolName;

    std::vector<ThreadPoolPtr> mWorkers;
};

typedef std::shared_ptr<HashThreadPool> HashThreadPoolPtr;


} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_THREAD_POOL_H
