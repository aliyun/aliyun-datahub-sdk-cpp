#ifndef DATAHUB_DATAHUB_SUBSCRIPTION_OFFSET_H
#define DATAHUB_DATAHUB_SUBSCRIPTION_OFFSET_H

#include <string>

namespace aliyun
{
namespace datahub
{

class SubscriptionOffset
{
public:
    SubscriptionOffset(
            int64_t timestamp=-1l,
            int64_t sequence=-1l,
            uint32_t batchIndex=0u,
            int64_t version=0l,
            int64_t sessionId=0l);

    ~SubscriptionOffset();

    void SetTimestamp(int64_t timestamp);
    void SetSequence(int64_t sequence);
    void SetBatchIndex(uint32_t batchIndex);
    void SetVersion(int64_t version);
    void SetSessionId(int64_t sessionId);

    int64_t GetTimestamp() const;
    int64_t GetSequence() const;
    uint32_t GetBatchIndex() const;
    int64_t GetVersion() const;
    int64_t GetSessionId() const;

    SubscriptionOffset& operator= (const SubscriptionOffset& offset)
    {
        mOffsetValue = offset.mOffsetValue;
        mVersion = offset.mVersion;
        mSessionId = offset.mSessionId;
        return *this;
    }

private:
    struct OffsetValue
    {
        OffsetValue(int64_t timestamp=-1l, int64_t sequence=-1l, uint32_t batchIndex=0u) : mTimestamp(timestamp), mSequence(sequence), mBatchIndex(batchIndex) {}
        int64_t mTimestamp;
        int64_t mSequence;
        uint32_t mBatchIndex;
    };

protected:
    OffsetValue mOffsetValue;
    int64_t mVersion;
    int64_t mSessionId;

private:
    friend class JsonTool;
};

} // namespace datahub
} // namespace aliyun

#endif //DATAHUB_DATAHUB_SUBSCRIPTION_OFFSET_H
