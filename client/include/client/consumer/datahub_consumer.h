#ifndef INCLUDE_DATAHUB_CLIENT_DATAHUB_CONSUMER_H
#define INCLUDE_DATAHUB_CLIENT_DATAHUB_CONSUMER_H

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
class OffsetCoordinator;
typedef std::shared_ptr<OffsetCoordinator> OffsetCoordinatorPtr;
class ShardGroupReader;
typedef std::shared_ptr<ShardGroupReader> ShardGroupReaderPtr;
class ConsumerConfiguration;
class RecordResult;
typedef std::shared_ptr<RecordResult> RecordResultPtr;

class DatahubConsumer
{
public:
    DatahubConsumer(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf);
    ~DatahubConsumer();

    TopicMetaPtr GetTopicMeta() const;

    RecordResultPtr Read();
    RecordResultPtr Read(const std::string& shardId);
    RecordResultPtr Read(int64_t timeout);
    RecordResultPtr Read(const std::string& shardId, int64_t timeout);

private:
    log4cpp::Category* mLoggerPtr;
    OffsetCoordinatorPtr mCoordinatorPtr;
    ShardGroupReaderPtr mShardGroupReaderPtr;
};

} // namespace datahub
} // namespace aliyun

#endif