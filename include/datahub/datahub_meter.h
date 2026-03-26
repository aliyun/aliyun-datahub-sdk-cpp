#ifndef DATAHUB_DATAHUB_METER_H
#define DATAHUB_DATAHUB_METER_H

namespace aliyun
{
namespace datahub
{

class MeterRecord
{
public:
    MeterRecord();
    ~MeterRecord();

    int64_t GetActiveTime() const;
    int64_t GetStorageSize() const;
    int64_t GetReadDataSize() const;
    int64_t GetWriteDataSize() const;
    int64_t GetReadTimes() const;
    int64_t GetWriteTimes() const;
    int64_t GetConnectorDataSize() const;
    int64_t GetStartTime() const;
    int64_t GetEndTime() const;

private:
    friend class JsonTool;
    int64_t mActiveTime;
    int64_t mStorageSize;
    int64_t mReadDataSize;
    int64_t mWriteDataSize;
    int64_t mReadTimes;
    int64_t mWriteTimes;
    int64_t mConnectorDataSize;
    int64_t mStartTime;
    int64_t mEndTime;
};

}
}



#endif //DATAHUB_DATAHUB_METER_H
