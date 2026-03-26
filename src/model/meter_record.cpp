#include <string>
#include "datahub/datahub_meter.h"

namespace aliyun
{
namespace datahub
{

MeterRecord::MeterRecord()
        : mActiveTime(0),
          mStorageSize(0),
          mReadDataSize(0),
          mWriteDataSize(0),
          mReadTimes(0),
          mWriteTimes(0),
          mConnectorDataSize(0),
          mStartTime(0),
          mEndTime(0)
{
}

MeterRecord::~MeterRecord()
{
}

int64_t MeterRecord::GetActiveTime() const
{
    return mActiveTime;
}

int64_t MeterRecord::GetStorageSize() const
{
    return mStorageSize;
}

int64_t MeterRecord::GetReadDataSize() const
{
    return mReadDataSize;
}

int64_t MeterRecord::GetWriteDataSize() const
{
    return mWriteDataSize;
}

int64_t MeterRecord::GetReadTimes() const
{
    return mReadTimes;
}

int64_t MeterRecord::GetWriteTimes() const
{
    return mWriteTimes;
}

int64_t MeterRecord::GetConnectorDataSize() const
{
    return mConnectorDataSize;
}

int64_t MeterRecord::GetStartTime() const
{
    return mStartTime;
}

int64_t MeterRecord::GetEndTime() const
{
    return mEndTime;
}

} // namespace datahub
} // namespace aliyun
