#include "datahub_client_factory.h"
#include "util.h"
#include "datahub/datahub_config.h"
#include "datahub/datahub_client.h"


namespace aliyun
{
namespace datahub
{

DatahubClientFactory::DatahubClientFactory() {}

DatahubClientPtr DatahubClientFactory::GetDatahubClient(const Configuration& conf)
{
    std::string clientKey;
    GenUtil::GenKey(clientKey, {conf.GetEndpoint(), conf.GetAccount().id, conf.GetAccount().key});
    if (mDatahubClientPtrMap.count(clientKey) == 0)
    {
        std::lock_guard<std::mutex> lock(mCreateMutex);
        if (mDatahubClientPtrMap.count(clientKey) == 0)
        {
            mDatahubClientPtrMap[clientKey] = std::make_shared<DatahubClient>(conf);
        }
    }
    return mDatahubClientPtrMap.at(clientKey);
}

DatahubClientFactory& DatahubClientFactory::GetInstance()
{
    static DatahubClientFactory sDatahubClientFactory;
    return sDatahubClientFactory;
}

} // namespace datahub
} // namespace aliyun
