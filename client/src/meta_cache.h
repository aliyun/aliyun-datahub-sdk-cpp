#ifndef INCLUDE_DATAHUB_CLIENT_META_CACHE_H
#define INCLUDE_DATAHUB_CLIENT_META_CACHE_H

#include <mutex>
#include <string>
#include <memory>
#include <unordered_map>


namespace aliyun
{
namespace datahub
{

class CommonConfiguration;
class ShardCoordinator;
class MetaData;
typedef std::shared_ptr<MetaData> MetaDataPtr;

class MetaCache
{
public:
    MetaDataPtr GetMetaData(const std::string& project, const std::string& topic, const std::string& subId, ShardCoordinator* coordinatorPtr, const CommonConfiguration& commonConf);
    static MetaCache& GetInstance();

private:
    MetaCache();

private:
    std::unordered_map<std::string, MetaDataPtr> mMetaDataCache;      // endpoint:project:topic   -->    MetaData
    std::mutex mMutex;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_META_CACHE_H