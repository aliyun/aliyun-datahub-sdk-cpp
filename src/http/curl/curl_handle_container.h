#ifndef DATAHUB_SDK_CURL_HANDLE_CONTAINER_H
#define DATAHUB_SDK_CURL_HANDLE_CONTAINER_H

#include <memory>
#include <stack>
#include <iostream>
#include <utility>
#include <pthread.h>
#include "curl/curl.h"

namespace aliyun
{
namespace datahub
{
namespace http
{

class CurlHandleContainer;

class CurlHandleWrapper
{
public:
    CurlHandleWrapper(CURL* handle, CurlHandleContainer* container)
        :mHandle(handle),mContainer(container)
    {}

    ~CurlHandleWrapper();

    CURL* GetHandle() { return mHandle; }
private:
    CURL* mHandle;
    CurlHandleContainer* mContainer;
};

typedef std::shared_ptr<CurlHandleWrapper> CurlHandleWrapperPtr;

class CurlHandleContainer
{
public:
    CurlHandleContainer(unsigned maxPoolSize = 50, long requestTimeout = 30000, long connectTimeout = 10000);
    ~CurlHandleContainer();

    CurlHandleWrapperPtr AcquireCurlHandle();

    void ReleaseCurlHandle(CURL* handle);

private:
    CurlHandleContainer(const CurlHandleContainer&);
    const CurlHandleContainer& operator = (const CurlHandleContainer&);

    bool CheckAndGrowPool();
    void SetDefaultOptionsOnHandle(void* handle);

    std::stack<CURL*> mMandleContainer;
    pthread_mutex_t mMandleContainerMutex;
    pthread_cond_t mConditionVariable;
    unsigned mMaxPoolSize;
    unsigned long mRequestTimeout;
    unsigned long mConnectTimeout;
    unsigned mPoolSize;
};

} // namespace http
} // namespace datahub
} // namespace aliyun
#endif
