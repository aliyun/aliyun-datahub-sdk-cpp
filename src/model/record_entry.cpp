#include <string>
#include "datahub/datahub_record.h"
#include "utils.h"

namespace aliyun
{
namespace datahub
{

RecordEntry::RecordEntry(
        const StringMap& attributes,
        const StringVec& values) :
    mAttributes(attributes),
    mRecordType(TUPLE)
{
    mFieldData.resize(values.size());
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (!values[i].empty())
        {
            mFieldData[i].SetValue(values[i]);
        }
    }
}

RecordEntry::RecordEntry(int fieldCount) :
    mAttributes(),
    mFieldData(fieldCount),
    mRecordType(TUPLE)
{
}

RecordEntry::RecordEntry(RecordType recordType) :
    mRecordType(recordType)
{
}

RecordEntry::~RecordEntry()
{
}

bool RecordEntry::SetString(uint32_t index, const std::string& value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetTinyint(uint32_t index, int8_t value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(std::to_string(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetSmallint(uint32_t index, int16_t value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(std::to_string(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetBigint(uint32_t index, int64_t value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(std::to_string(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetDouble(uint32_t index, double value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(Utils::DoubleToString(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetTimestamp(uint32_t index, int64_t value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(std::to_string(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetBoolean(uint32_t index, bool value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(value ? "true" : "false");
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetDecimal(uint32_t index, const std::string& value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetInteger(uint32_t index, int32_t value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(std::to_string(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetFloat(uint32_t index, float value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(Utils::FloatToString(value));
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetJson(uint32_t index, const std::string& value)
{
    if (mRecordType == TUPLE && index < mFieldData.size())
    {
        mFieldData[index].SetValue(value);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetData(const std::string& data)
{
    if (mRecordType == BLOB)
    {
        mBlobValue.assign(data);
        return true;
    }
    else
    {
        return false;
    }
}

bool RecordEntry::SetData(const char* data, int len)
{
    if (mRecordType == BLOB)
    {
        mBlobValue.assign(data, len);
        return true;
    }
    else
    {
        return false;
    }
}

void RecordEntry::SetAttribute(const std::string& key, const std::string& value)
{
    mAttributes[key] = value;
}

void RecordEntry::SetShardId(const std::string& shardId)
{
    mShardId = shardId;
}

void RecordEntry::SetHashValue(const std::string& hashValue)
{
    mHashValue = hashValue;
}

void RecordEntry::SetPartitionKey(const std::string& partitionKey)
{
    mPartitionKey = partitionKey;
}

uint32_t RecordEntry::GetTotalSize(void) const
{
    if (mRecordType == BLOB)
    {
        return mBlobValue.length();
    }
    else
    {
        uint32_t size = 0;
        for (size_t i = 0; i < mFieldData.size(); i++)
        {
            size += mFieldData[i].GetValue().size();
        }
        return size;
    }
}

} // namespace datahub
} // namespace aliyun
