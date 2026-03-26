#include "client/producer/datahub_producer.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "logger.h"
#include "meta_data.h"
#include "shard_coordinator.h"
#include "shard_group_writer.h"
#include "datahub/datahub_record.h"
#include "client/write_result.h"
#include "client/producer/producer_config.h"

namespace aliyun
{
namespace datahub
{

DatahubProducer::DatahubProducer(const std::string& projectName, const std::string& topicName, const ProducerConfiguration& producerConf, const StringVec& shardIds)
{
    Logger::GetInstance().SetLogLevel(producerConf.GetLogPriorityLevel());
    Logger::GetInstance().SetLogFilePath(producerConf.GetLogFilePath());
    mLoggerPtr = Logger::GetInstance().GenLogger("producer");

    mCoordinatorPtr = std::make_shared<ShardCoordinator>(projectName, topicName, "", (CommonConfiguration)producerConf);
    LOG_INFO(mLoggerPtr, "Create ShardCoordinator success. key: %s", mCoordinatorPtr->GetUniqKey().c_str());

    mShardGroupWriterPtr = std::make_shared<ShardGroupWriter>(mCoordinatorPtr.get(), shardIds, producerConf);
    LOG_INFO(mLoggerPtr, "Create ShardGroupWriter success. key: %s, shardIds: %s", mCoordinatorPtr->GetUniqKey().c_str(), PrintUtil::GetMsg(shardIds).c_str());

    mCoordinatorPtr->RegisterOnShardChange(std::bind(&ShardGroupWriter::OnShardChange, mShardGroupWriterPtr, std::placeholders::_1, std::placeholders::_2));
    mCoordinatorPtr->RegisterOnRemoveAllShards(std::bind(&ShardGroupWriter::OnRemoveAllShards, mShardGroupWriterPtr));
}

DatahubProducer::~DatahubProducer()
{
    LOG_INFO(mLoggerPtr, "DatahubProducer close success. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
}

TopicMetaPtr DatahubProducer::GetTopicMeta() const
{
    return mCoordinatorPtr->GetMetaData()->GetTopicMeta();
}

std::string DatahubProducer::Write(const RecordEntryVec& records)
{
    return mShardGroupWriterPtr->Write(records);
}

WriteResultFuturePtr DatahubProducer::WriteAsync(const RecordEntryVec& records)
{
    return mShardGroupWriterPtr->WriteAsync(records);
}

void DatahubProducer::Flush()
{
    mShardGroupWriterPtr->Flush();
}

} // namespace datahub
} // namespace aliyun
