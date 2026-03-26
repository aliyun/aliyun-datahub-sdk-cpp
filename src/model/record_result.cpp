#include <algorithm>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

RecordResult::RecordResult() :
    mSystemTime(-1),
    mSequence(-1)
{
}

RecordResult::~RecordResult()
{
}

int64_t RecordResult::GetSystemTime() const
{
    return mSystemTime;
}

std::string RecordResult::GetCursor() const
{
    if (mCursor.empty())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Not receive cursor from server, maybe server version is older.");
    }
    return mCursor;
}

int64_t RecordResult::GetSequence() const
{
    if (mSequence < 0)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Not receive sequence from server, maybe server version is older.");
    }
    return mSequence;
}


const StringMap& RecordResult::GetAttributes() const
{
    return mAttributes;
}

const FieldDataVec& RecordResult::GetFieldData() const
{
    return mFieldData;
}

std::string RecordResult::GetString(uint32_t index) const
{
    CheckNotNull(index);
    return mFieldData[index].GetValue();
}

bool RecordResult::IsNull(uint32_t index) const
{
    CheckIndex(index);
    return mFieldData[index].IsNull();
}

int8_t RecordResult::GetTinyint(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        int32_t ret = std::stoi(mFieldData[index].GetValue());
        if (ret > INT8_MAX || ret < INT8_MIN)
        {
            throw DatahubException(LOCAL_ERROR_CODE, "index [" + std::to_string(index)
                                                     + "] invalid tiny int value: " + mFieldData[index].GetValue());
        }
        return (int8_t)ret;
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

int16_t RecordResult::GetSmallint(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        int32_t ret = std::stoi(mFieldData[index].GetValue());
        if (ret > INT16_MAX || ret < INT16_MIN)
        {
            throw DatahubException(LOCAL_ERROR_CODE, "index [" + std::to_string(index)
                                                     + "] invalid small int value: " + mFieldData[index].GetValue());
        }
        return (int16_t)ret;
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

std::string RecordResult::GetDecimal(uint32_t index) const
{
    CheckNotNull(index);
    return mFieldData[index].GetValue();
}

int64_t RecordResult::GetBigint(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        int64_t ret = std::stoll(mFieldData[index].GetValue());
        return ret;
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

double RecordResult::GetDouble(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        double ret = std::stod(mFieldData[index].GetValue());
        return ret;
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

int64_t RecordResult::GetTimestamp(uint32_t index) const
{
    return GetBigint(index);
}

bool RecordResult::GetBoolean(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        const std::string& value = mFieldData[index].GetValue();
        std::string lower(value);
        std::transform(value.begin(), value.end(), lower.begin(), ::tolower);
        if (lower == "true")
        {
            return true;
        }
        else if (lower == "false")
        {
            return false;
        }
        else
        {
            throw DatahubException(LOCAL_ERROR_CODE, "invalid bool value:" + value);
        }
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

int32_t RecordResult::GetInteger(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        int32_t ret = std::stoi(mFieldData[index].GetValue());
        return ret;
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

float RecordResult::GetFloat(uint32_t index) const
{
    try
    {
        CheckNotNull(index);
        float ret = std::stof(mFieldData[index].GetValue());
        return ret;
    }
    catch (const std::exception& e)
    {
        throw DatahubException(LOCAL_ERROR_CODE, e.what());
    }
}

std::string RecordResult::GetJson(uint32_t index) const
{
    CheckNotNull(index);
    return mFieldData[index].GetValue();
}

const char* RecordResult::GetData(int& len) const
{
    len = mBlobValue.size();
    return mBlobValue.c_str();
}

std::string RecordResult::GetData(void) const
{
    return mBlobValue;
}

void RecordResult::CheckIndex(uint32_t index) const
{
    if (index >= mFieldData.size())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "invalid index value");
    }
}

void RecordResult::CheckNotNull(uint32_t index) const
{
    CheckIndex(index);
    if (mFieldData[index].IsNull())
    {
        throw DatahubException(LOCAL_ERROR_CODE, "value is null");
    }
}

uint32_t RecordResult::GetTotalSize(void) const
{
    uint32_t size = 0;
    for (size_t i = 0; i < mFieldData.size(); i++)
    {
        size += mFieldData[i].GetValue().size();
    }

    return size;
}

void RecordResult::SetMessageKey(MessageKeyPtr messageKey)
{
    mMessageKey = messageKey;
}

MessageKeyPtr RecordResult::GetMessageKey()
{
    return mMessageKey;
}

} // namespace datahub
} // namespace aliyun
