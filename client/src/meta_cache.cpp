#include "meta_cache.h"
#include "util.h"
#include "meta_data.h"
#include "shard_coordinator.h"
#include "client/common_config.h"


namespace aliyun
{
namespace datahub
{

MetaCache::MetaCache() {}

MetaDataPtr MetaCache::GetMetaData(const std::string& project, const std::string& topic, const std::string& subId, ShardCoordinator* coordinatorPtr, const CommonConfiguration& commonConf)
{
    std::string key;
    GenUtil::GenKey(key, {commonConf.GetEndpoint(), project, topic, subId});
    if (mMetaDataCache.count(key) == 0)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mMetaDataCache.count(key) == 0)
        {
            MetaDataPtr metaData = std::make_shared<MetaData>(project, topic, subId, commonConf);
            metaData->Init();
            mMetaDataCache[key] = metaData;
        }
    }
    MetaDataPtr metaData = mMetaDataCache.at(key);
    metaData->Register(coordinatorPtr);
    return metaData;
}

MetaCache& MetaCache::GetInstance()
{
    static MetaCache sMetaCache;
    return sMetaCache;
}

} // namespace datahub
} // namespace aliyun
