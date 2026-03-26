#include "shard_group_writer.h"
#include "log4cpp/Category.hh"
#include "util.h"
#include "timer.h"
#include "logger.h"
#include "meta_data.h"
#include "shard_meta.h"
#include "shard_writer.h"
#include "message_writer.h"
#include "record_pack.h"
#include "record_pack_queue.h"
#include "shard_coordinator.h"
#include "shard_select_strategy.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_shard.h"
#include "datahub/datahub_exception.h"
#include "client/write_result.h"
#include <algorithm>

namespace aliyun
{
namespace datahub
{

ShardGroupWriter::ShardGroupWriter(ShardCoordinator* coordinator, const StringVec& shardIds, const ProducerConfiguration& producerConf)
    : mClosed(false),
      mProducerConf(producerConf),
      mCoordinatorPtr(coordinator),
      mMessageWriterPtr(mCoordinatorPtr->GetMetaData()->GetMessageWriter())
{
    mLoggerPtr = Logger::GetInstance().GenLogger("shardGroupWriter");
    mCoordinatorPtr->SetAssignShards(shardIds);
    mShardSelector.reset(new ShardWriterSelectStrategy());
    CreateShardWriterWhenInit(shardIds);
}

ShardGroupWriter::~ShardGroupWriter()
{
    mClosed = true;
    RemoveAllShardWriter();
}

std::string ShardGroupWriter::Write(const RecordEntryVec& records)
{
    if (mClosed)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "ShardGroupWriter closed. key: " + mCoordinatorPtr->GetUniqKey());
    }

    CheckRecords(records);
    mCoordinatorPtr->UpdateShardInfo();

    int maxRetry = mShardWriterMap.size();
    for (int i = 0; i < maxRetry; i++)
    {
        ShardWriterPtr writer = GetNextWriter();
        try
        {
            writer->Write(records);
            return writer->GetShardId();
        }
        catch (const DatahubException& e)
        {
            LOG_WARN(mLoggerPtr, "ShardGroupWriter write failed, check it please. key: %s, shardId: %s, %s", mCoordinatorPtr->GetUniqKey().c_str(), writer->GetShardId().c_str(), e.GetErrorMessage().c_str());
            mShardSelector->AddInvalidShard(writer->GetShardId());
        }
    }

    throw DatahubException(LOCAL_ERROR_CODE, "No valid shard to write, check it please. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
}

WriteResultFuturePtr ShardGroupWriter::WriteAsync(const RecordEntryVec& records)
{
    if (mClosed)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "ShardGroupWriter closed. key: " + mCoordinatorPtr->GetUniqKey());
    }

    CheckRecords(records);
    mCoordinatorPtr->UpdateShardInfo();

    WriteResultFuturePtr result = nullptr;
    while (!result)
    {
        ShardWriterPtr writer = GetNextWriter();
        result = writer->WriteAsync(records);
    }
    return result;
}

void ShardGroupWriter::Flush()
{
    // Lock for ShardWriterMap
    std::unique_lock<std::mutex> lock(mShardWriterMutex);
    for (auto it = mShardWriterMap.begin(); it != mShardWriterMap.end(); it++)
    {
        it->second->Flush();
    }
}

void ShardGroupWriter::OnShardChange(const StringVec& addShards, const StringVec& delShards)
{
    LOG_DEBUG(mLoggerPtr, "Callback shard writer change. key: %s, add shardIds: %s, del shardIds: %s", mCoordinatorPtr->GetUniqKey().c_str(), PrintUtil::GetMsg(addShards).c_str(), PrintUtil::GetMsg(delShards).c_str());
    CreateShardWriter(addShards);
    RemoveShardWriter(delShards);
}

void ShardGroupWriter::OnRemoveAllShards()
{
    LOG_DEBUG(mLoggerPtr, "Callback remove all shards writer. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
    RemoveAllShardWriter();
}

void ShardGroupWriter::CreateShardWriterWhenInit(const StringVec& shardIds)
{
    if (!shardIds.empty())
    {
        CreateShardWriter(shardIds);
        return ;
    }

    StringVec shardIdFromMeta;
    const auto& shardMetaMap = mCoordinatorPtr->GetMetaData()->GetShardMetaMap();
    for (auto it = shardMetaMap.begin(); it != shardMetaMap.end(); it++)
    {
        if (it->second->GetShardState() == ACTIVE)
        {
            shardIdFromMeta.push_back(it->first);
        }
    }
    CreateShardWriter(shardIdFromMeta);
}

void ShardGroupWriter::CreateShardWriter(const StringVec& shardIds)
{
    std::unique_lock<std::mutex> lock(mShardWriterMutex);
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (mShardWriterMap.count(*it) == 0)
        {
            // For mShardWriterMap
            mShardWriterMap[*it] = std::make_shared<ShardWriter>(mCoordinatorPtr->GetProjectName(), mCoordinatorPtr->GetTopicName(), mProducerConf, *it, mMessageWriterPtr, mShardSelector);
            // For mShardSelector
            mShardSelector->AddShard(*it);
            LOG_INFO(mLoggerPtr, "ShardWriter created. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), it->c_str());
        }
    }
}

void ShardGroupWriter::RemoveShardWriter(const StringVec& shardIds)
{
    std::unique_lock<std::mutex> lock(mShardWriterMutex);
    for (auto it = shardIds.begin(); it != shardIds.end(); it++)
    {
        if (mShardWriterMap.count(*it) > 0)
        {
            // For mShardWriterMap
            mShardWriterMap.erase(*it);
            // For mShardSelector
            mShardSelector->RemoveShard(*it);
            LOG_INFO(mLoggerPtr, "ShardWriter removed. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), it->c_str());
        }
    }
}

void ShardGroupWriter::RemoveAllShardWriter()
{
    std::unique_lock<std::mutex> lock(mShardWriterMutex);
    mShardWriterMap.clear();
    mShardSelector->ClearAllShard();
    LOG_INFO(mLoggerPtr, "ShardWriter clear all. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
}

ShardWriterPtr ShardGroupWriter::GetNextWriter()
{
    const std::string& nextShardId = mShardSelector->GetNextShard();
    if (nextShardId.empty())
    {
        if (mShardWriterMap.empty())
        {
            LOG_WARN(mLoggerPtr, "No active ShardWriter found. key: %s", mCoordinatorPtr->GetUniqKey().c_str());
            throw DatahubException(LOCAL_ERROR_CODE, "No active shard found. key: " + mCoordinatorPtr->GetUniqKey());
        }
        else
        {
            return mShardWriterMap.begin()->second;
        }
    }

    return GetShardWriter(nextShardId);
}

ShardWriterPtr ShardGroupWriter::GetShardWriter(const std::string& shardId)
{
    std::unique_lock<std::mutex> lock(mShardWriterMutex);
    const auto& writerIt = mShardWriterMap.find(shardId);
    if (writerIt != mShardWriterMap.end())
    {
        return writerIt->second;
    }
    else
    {
        LOG_WARN(mLoggerPtr, "ShardWriter not found. key: %s, shardId: %s", mCoordinatorPtr->GetUniqKey().c_str(), shardId.c_str());
        throw DatahubException(LOCAL_ERROR_CODE, "ShardWriter not found. key: " + mCoordinatorPtr->GetUniqKey() + ", shardId: " + shardId);
    }
}

void ShardGroupWriter::CheckRecords(const RecordEntryVec& records)
{
    for (auto it = records.begin(); it != records.end(); it++)
    {
        if (!(it->GetShardId().empty() && it->GetPartitionKey().empty() && it->GetHashValue().empty()))
        {
            LOG_WARN(mLoggerPtr, "Client producer not support put record by special shardId, partitionKey, hashKey. key: %s, shardId: %s, partitionKey: %s, hashKey: %s",
                    mCoordinatorPtr->GetUniqKey().c_str(), it->GetShardId().c_str(), it->GetPartitionKey().c_str(), it->GetHashValue().c_str());
            throw DatahubException(LOCAL_ERROR_CODE, "Client producer not support put record by special shardId, partitionKey, hashKey");
        }
    }
}

} // namespace datahub
} // namespace aliyun
