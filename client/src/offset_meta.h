#ifndef INCLUDE_DATAHUB_CLIENT_OFFSET_META_H
#define INCLUDE_DATAHUB_CLIENT_OFFSET_META_H

#include <string>


namespace aliyun
{
namespace datahub
{

class OffsetMeta
{
public:
    OffsetMeta() : OffsetMeta(-1l, -1l) {}

    OffsetMeta(int64_t version, int64_t sessionId)
        : mVersion(version),
          mSessionId(sessionId)
    {}

    int64_t GetVersion() const { return mVersion; }
    int64_t GetSessionId() const { return mSessionId; }

    void SetVersion(int64_t version) { mVersion = version; }
    void SetSessionId(int64_t sessionId) { mSessionId = sessionId; }

protected:
    int64_t mVersion;
    int64_t mSessionId;
};

class ConsumeOffsetMeta : public OffsetMeta
{
public:
    ConsumeOffsetMeta() : ConsumeOffsetMeta(-1l, -1l, -1l, -1l) {}

    ConsumeOffsetMeta(int64_t timestamp, int64_t sequence, int64_t version, int64_t sessionId)
        : OffsetMeta(version, sessionId),
          mTimestamp(timestamp),
          mSequence(sequence),
          mNextCursor("")
    {}

    int64_t GetSequence() const { return mSequence; }
    int64_t GetTimestamp() const { return mTimestamp; }
    std::string GetNextCursor() const { return mNextCursor; }
    void SetNextCursor(const std::string& nextCursor) { mNextCursor = nextCursor; }

    void ResetTimestamp(int64_t timestamp)
    {
        mNextCursor.clear();
        mTimestamp = timestamp;
        mSequence = -1l;
    }

private:
    int64_t mTimestamp;
    int64_t mSequence;
    std::string mNextCursor;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_OFFSET_META_H
