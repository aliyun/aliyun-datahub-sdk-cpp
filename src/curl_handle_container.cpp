#include "http/curl/curl_handle_container.h"

class LibCurlInitializer
{
public:
    LibCurlInitializer()
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }
    ~LibCurlInitializer()
    {
        curl_global_cleanup();
    }
};

static LibCurlInitializer sInit;

namespace aliyun
{
namespace datahub
{
namespace http
{

CurlHandleWrapper::~CurlHandleWrapper()
{
    if (mHandle)
    {
        mContainer->ReleaseCurlHandle(mHandle);
    }
}

CurlHandleContainer::CurlHandleContainer(unsigned maxPoolSize, long requestTimeout, long connectTimeout) :
                mMaxPoolSize(maxPoolSize), mRequestTimeout(requestTimeout), mConnectTimeout(connectTimeout),
                mPoolSize(0)
{
    pthread_mutex_init(&mMandleContainerMutex, NULL);
    pthread_cond_init(&mConditionVariable, NULL);
}

CurlHandleContainer::~CurlHandleContainer()
{
    pthread_mutex_lock(&mMandleContainerMutex);
    while (mMandleContainer.size() > 0)
    {
        curl_easy_cleanup(mMandleContainer.top());
        mMandleContainer.pop();
    }
    pthread_mutex_unlock(&mMandleContainerMutex);
}

CurlHandleWrapperPtr CurlHandleContainer::AcquireCurlHandle()
{
    pthread_mutex_lock(&mMandleContainerMutex);

    while (mMandleContainer.size() == 0)
    {
        if (!CheckAndGrowPool())
        {
            pthread_cond_wait(&mConditionVariable, &mMandleContainerMutex);
        }
    }

    CURL* handle = mMandleContainer.top();
    mMandleContainer.pop();
    pthread_mutex_unlock(&mMandleContainerMutex);
    return CurlHandleWrapperPtr(new CurlHandleWrapper(handle, this));
}

void CurlHandleContainer::ReleaseCurlHandle(CURL* handle)
{
    if (handle)
    {
        curl_easy_reset(handle);
        SetDefaultOptionsOnHandle(handle);
        pthread_mutex_lock(&mMandleContainerMutex);
        mMandleContainer.push(handle);
        pthread_cond_signal(&mConditionVariable);
        pthread_mutex_unlock(&mMandleContainerMutex);
    }
}

bool CurlHandleContainer::CheckAndGrowPool()
{
    if (mPoolSize < mMaxPoolSize)
    {
        unsigned multiplier = mPoolSize > 0 ? mPoolSize : 1;
        unsigned amountToAdd = std::min(multiplier * 2, mMaxPoolSize - mPoolSize);

        unsigned actuallyAdded = 0;
        for (unsigned i = 0; i < amountToAdd; ++i)
        {
            CURL* curlHandle = curl_easy_init();

            if (curlHandle)
            {
                SetDefaultOptionsOnHandle(curlHandle);
                mMandleContainer.push(curlHandle);
                ++actuallyAdded;
            }
            else
            {
                //TODO
            }
        }

        mPoolSize += actuallyAdded;

        return actuallyAdded > 0;
    }

    return false;
}

void CurlHandleContainer::SetDefaultOptionsOnHandle(void* handle)
{
    curl_easy_setopt(handle, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, mRequestTimeout);
    curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, mConnectTimeout);
}

} // namespace http
} // namespace datahub
} // namespace aliyun
