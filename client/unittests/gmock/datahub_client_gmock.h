#ifndef INCLUDE_DATAHUB_CLIENT_DATAHUB_CLIENT_GMOCK_H
#define INCLUDE_DATAHUB_CLIENT_DATAHUB_CLIENT_GMOCK_H

#include "gmock/gmock.h"
#include "datahub/datahub_client.h"

namespace aliyun
{
namespace datahub
{

class GMockDatahubClient : public DatahubClient
{
public:
    GMockDatahubClient(const Configuration& conf) : DatahubClient(conf) {}
    virtual ~GMockDatahubClient() {}

    MOCK_METHOD4(JoinGroup, JoinGroupResult (const std::string&, const std::string&, const std::string&, int64_t));
    MOCK_METHOD7(Heartbeat, HeartbeatResult (const std::string&, const std::string&, const std::string&, const std::string&, int64_t, const StringVec&, const StringVec&));
    MOCK_METHOD7(SyncGroup, void (const std::string&, const std::string&, const std::string&, const std::string&, int64_t, const StringVec&, const StringVec&));
    MOCK_METHOD5(LeaveGroup, void (const std::string&, const std::string&, const std::string&, const std::string&, int64_t));

    MOCK_METHOD2(GetTopic, GetTopicResult (const std::string&, const std::string&));
    MOCK_METHOD2(ListShard, ListShardResult (const std::string&, const std::string&));

    MOCK_METHOD4(InitSubscriptionOffsetSession, OpenSubscriptionOffsetSessionResult (const std::string&, const std::string&, const std::string&, const StringVec&));
    MOCK_METHOD4(UpdateSubscriptionOffset, void (const std::string&, const std::string&, const std::string&, const std::map<std::string, SubscriptionOffset>&));

    MOCK_METHOD4(GetCursor, GetCursorResult (const std::string&, const std::string&, const std::string&, const CursorType&));
    MOCK_METHOD5(GetCursor, GetCursorResult (const std::string&, const std::string&, const std::string&, const CursorType&, int64_t));
    MOCK_METHOD6(GetRecord, GetRecordResult (const std::string&, const std::string&, const std::string&, const std::string&, int, const std::string&));

    MOCK_METHOD4(PutRecordByShard, PutRecordByShardResult (const std::string&, const std::string&, const std::string&, const RecordEntryVec&));
};

typedef std::shared_ptr<GMockDatahubClient> GMockDatahubClientPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_DATAHUB_CLIENT_GMOCK_H