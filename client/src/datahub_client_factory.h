#ifndef INCLUDE_DATAHUB_CLIENT_DATAHUB_CLIENT_FACTORY_H
#define INCLUDE_DATAHUB_CLIENT_DATAHUB_CLIENT_FACTORY_H

#include <mutex>
#include <memory>
#include <unordered_map>


namespace aliyun
{
namespace datahub
{

class Configuration;
class DatahubClient;
typedef std::shared_ptr<DatahubClient> DatahubClientPtr;

class DatahubClientFactory
{
public:
    DatahubClientPtr GetDatahubClient(const Configuration& conf);
    static DatahubClientFactory& GetInstance();

private:
    DatahubClientFactory();

private:
    std::unordered_map<std::string, DatahubClientPtr> mDatahubClientPtrMap;
    std::mutex mCreateMutex;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_DATAHUB_CLIENT_FACTORY_H