
#include "client/consumer/datahub_consumer.h"
#include "log4cpp/Category.hh"
#include "logger.h"
#include "meta_data.h"
#include "consumer_coordinator.h"
#include "shard_group_reader.h"
#include "datahub/datahub_record.h"
#include "client/consumer/consumer_config.h"


namespace aliyun
{
namespace datahub
{

DatahubConsumer::DatahubConsumer(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf)
{
    Logger::GetInstance().SetLogLevel(consumerConf.GetLogPriorityLevel());
    Logger::GetInstance().SetLogFilePath(consumerConf.GetLogFilePath());
    mLoggerPtr = Logger::GetInstance().GenLogger("consumer");

    mCoordinatorPtr = std::make_shared<ConsumerCoordinator>(projectName, topicName, subId, consumerConf);
    LOG_INFO(mLoggerPtr, "Create ConsumerCoordinator success. key: %s", mCoordinatorPtr->GetUniqKey().c_str());

    mShardGroupReaderPtr = std::make_shared<ShardGroupReader>(mCoordinatorPtr.get(), consumerConf, StringVec(), -1l);
    LOG_INFO(mLoggerPtr, "Create ShardGroupReader success. key: %s", mCoordinatorPtr->GetUniqKey().c_str());

    mCoordinatorPtr->RegisterOnShardChange(std::bind(&ShardGroupReader::OnShardChange, mShardGroupReaderPtr, std::placeholders::_1, std::placeholders::_2));
    mCoordinatorPtr->RegisterOnRemoveAllShards(std::bind(&ShardGroupReader::OnRemoveAllShards, mShardGroupReaderPtr));
}

DatahubConsumer::~DatahubConsumer()
{
    LOG_INFO(mLoggerPtr, "DatahubConsumer close success. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
}

TopicMetaPtr DatahubConsumer::GetTopicMeta() const
{
    return mCoordinatorPtr->GetMetaData()->GetTopicMeta();
}

RecordResultPtr DatahubConsumer::Read()
{
    return Read("", INT64_MAX);
}

RecordResultPtr DatahubConsumer::Read(const std::string& shardId)
{
    return Read(shardId, INT64_MAX);
}

RecordResultPtr DatahubConsumer::Read(int64_t timeout)
{
    return Read("", timeout);
}

RecordResultPtr DatahubConsumer::Read(const std::string& shardId, int64_t timeout)
{
    return mShardGroupReaderPtr->Read(shardId, timeout);
}

} // namespace datahub

} // namespace aliyun
