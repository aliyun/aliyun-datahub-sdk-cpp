#ifndef INCLUDE_DATAHUB_CLIENT_DATAHUB_PRODUCER_H
#define INCLUDE_DATAHUB_CLIENT_DATAHUB_PRODUCER_H

#include <vector>
#include <string>
#include <future>
#include <memory>
#include "client/topic_meta.h"
#include "datahub/datahub_typedef.h"

namespace log4cpp
{
    class Category;
}

namespace aliyun
{
namespace datahub
{

class Logger;
class ShardCoordinator;
typedef std::shared_ptr<ShardCoordinator> ShardCoordinatorPtr;
class ShardGroupWriter;
typedef std::shared_ptr<ShardGroupWriter> ShardGroupWriterPtr;
class ProducerConfiguration;
class RecordEntry;
typedef std::vector<RecordEntry> RecordEntryVec;
class WriteResult;
typedef std::shared_ptr<WriteResult> WriteResultPtr;
typedef std::shared_future<WriteResultPtr> WriteResultFuture;
typedef std::shared_ptr<WriteResultFuture> WriteResultFuturePtr;

class DatahubProducer
{
public:
    DatahubProducer(const std::string& projectName, const std::string& topicName, const ProducerConfiguration& producerConf, const StringVec& shardIds = StringVec());
    ~DatahubProducer();

    TopicMetaPtr GetTopicMeta() const;

    std::string Write(const RecordEntryVec& records);
    WriteResultFuturePtr WriteAsync(const RecordEntryVec& records);
    void Flush();

private:
    log4cpp::Category* mLoggerPtr;
    ShardCoordinatorPtr mCoordinatorPtr;
    ShardGroupWriterPtr mShardGroupWriterPtr;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_DATAHUB_PRODUCER_H
