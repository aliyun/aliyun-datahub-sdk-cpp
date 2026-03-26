#include <cstdlib>
#include <string>
#include "datahub/datahub_exception.h"
#include "protobuf_tool.h"

namespace aliyun
{
namespace datahub
{

void ProtobufTool::MessageToRecordResult(const aliyun::datahub::proto::RecordEntry& pbEntry, RecordResult& entry)
{
    if (pbEntry.has_attributes())
    {
        const aliyun::datahub::proto::RecordAttributes& attributes = pbEntry.attributes();
        for (int i = 0; i < attributes.attributes_size(); ++i)
        {
            const aliyun::datahub::proto::StringPair& attribute = attributes.attributes(i);
            entry.mAttributes[attribute.key()] = attribute.value();
        }
    }

    if (pbEntry.has_system_time())
    {
        entry.mSystemTime = pbEntry.system_time();
    }

    if (pbEntry.has_cursor())
    {
        entry.mCursor = pbEntry.cursor();
    }

    if (pbEntry.has_sequence())
    {
        entry.mSequence = pbEntry.sequence();
    }
    else
    {
        entry.mSequence = -1;
    }

    if (pbEntry.has_data())
    {
        const aliyun::datahub::proto::RecordData& data = pbEntry.data();
        entry.mFieldData.clear();
        entry.mFieldData.resize(data.data_size());
        for (int i = 0; i < data.data_size(); ++i)
        {
            if (data.data(i).has_value())
            {
                entry.mFieldData[i].SetValue(data.data(i).value());
            }
        }

        if (entry.mFieldData.size() == 1)
        {
            entry.mBlobValue = entry.mFieldData[0].GetValue();
        }
    }
}

void ProtobufTool::RecordEntryToMessage(const RecordEntry& entry, aliyun::datahub::proto::RecordEntry* pbEntry)
{

    if (!entry.mShardId.empty())
    {
        pbEntry->set_shard_id(entry.mShardId);
    }
    else if (!entry.mPartitionKey.empty())
    {
        pbEntry->set_partition_key(entry.mPartitionKey);
    }
    else if (!entry.mHashValue.empty())
    {
        pbEntry->set_hash_key(entry.mHashValue);
    }

    aliyun::datahub::proto::RecordAttributes* recordAttributes = pbEntry->mutable_attributes();
    std::map<std::string, std::string>::const_iterator iter;
    for (iter = entry.mAttributes.begin(); iter != entry.mAttributes.end(); ++iter)
    {
        aliyun::datahub::proto::StringPair* attribute = recordAttributes->add_attributes();
        attribute->set_key(iter->first);
        attribute->set_value(iter->second);
    }

    aliyun::datahub::proto::RecordData* recordData = pbEntry->mutable_data();
    if (entry.mRecordType == TUPLE)
    {
        FieldDataVec::const_iterator it;
        for (it = entry.mFieldData.begin(); it != entry.mFieldData.end(); ++it)
        {
            aliyun::datahub::proto::FieldData* data = recordData->add_data();
            if (!it->IsNull())
            {
                data->set_value(it->GetValue());
            }
        }
    }
    else
    {
        aliyun::datahub::proto::FieldData* data = recordData->add_data();
        data->set_value(entry.mBlobValue);
    }
}

void ProtobufTool::MessageToErrorEntry(const aliyun::datahub::proto::FailedRecord& failedRecord, ErrorEntry& entry)
{
    entry.mIndex = failedRecord.index();
    if (failedRecord.has_error_code())
    {
        entry.mErrorCode = failedRecord.error_code();
    }
    if (failedRecord.has_error_message())
    {
        entry.mErrorMessage = failedRecord.error_message();
    }
}

} // namespace datahub
} // namespace aliyun
