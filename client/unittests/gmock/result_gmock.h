#ifndef INCLUDE_DATAHUB_CLIENT_RESULT_GMOCK_H
#define INCLUDE_DATAHUB_CLIENT_RESULT_GMOCK_H

#include "datahub/datahub_result.h"

namespace aliyun
{
namespace datahub
{

class GMockResult
{
public:
    JoinGroupResult GetJoinGroupResult() { return mJoinGroupResult; }
    HeartbeatResult GetHeartbeatResult() { return mHeartbeatResult; }
    GetTopicResult GetGetTopicResult() { return mGetTopicResult; }
    ListShardResult GetListShardResult() { return mListShardResult; }
    OpenSubscriptionOffsetSessionResult GetOpenSubscriptionOffsetSessionResult() { return mOpenSubscriptionOffsetSessionResult; }
    GetCursorResult GetGetCursorResult() { return mGetCursorResult; }
    GetRecordResult GetGetRecordResult() { return mGetRecordResult; }
    
    // shard_reader_gtest
    GetRecordResult GetGetRecordResultWithZeroCount() { return mGetRecordResultWithZeroCount; }

    // consumer_heartbeat_gtest
    HeartbeatResult GetHeartbeatResultWithNull() { return mHeartbeatResultWithNull; }

    // meta_data_gtest
    ListShardResult GetListShardResult1() { return mListShardResult1; }
    ListShardResult GetListShardResult2() { return mListShardResult2; }
    ListShardResult GetListShardResult3() { return mListShardResult3; }

    static GMockResult& GetInstance();

private:
    GMockResult();

private:
    const std::string JOIN_GROUP_RESULT = "{\"ConsumerId\":\"test\",\"VersionId\":1,\"SessionTimeout\":60000}";
    const std::string HEARTBEAT_RESULT = "{\"ShardList\":[\"0\",\"1\",\"2\"],\"TotalPlan\":\"test\",\"PlanVersion\":1}";
    const std::string GET_TOPIC_RESULT = "{\"Comment\": \"test topic\",\"CreateTime\": 1525763481,\"LastModifyTime\": 1525763481,\"Lifecycle\": 1,\"RecordSchema\": \"{\\\"fields\\\":[{\\\"name\\\":\\\"f1\\\",\\\"type\\\":\\\"STRING\\\", \\\"comment\\\":\\\"f1 comment\\\"},{\\\"name\\\":\\\"f2\\\",\\\"type\\\":\\\"BIGINT\\\", \\\"comment\\\":\\\"f2 comment\\\"}]}\",\"RecordType\": \"TUPLE\",\"ShardCount\": 4}";
    const std::string LIST_SHARD_RESULT = "{\"Interval\":300000,\"Protocol\":\"http1.1\",\"Shards\":[{\"BeginHashKey\":\"00000000000000000000000000000000\",\"BeginKey\":\"00000000000000000000000000000000\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"55555555555555555555555555555555\",\"EndKey\":\"55555555555555555555555555555555\",\"LeftShardId\":\"4294967295\",\"ParentShardIds\":[],\"RightShardId\":\"1\",\"ShardId\":\"0\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"55555555555555555555555555555555\",\"BeginKey\":\"55555555555555555555555555555555\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"EndKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"LeftShardId\":\"0\",\"ParentShardIds\":[],\"RightShardId\":\"2\",\"ShardId\":\"1\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"BeginKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"EndKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"LeftShardId\":\"1\",\"ParentShardIds\":[],\"RightShardId\":\"4294967295\",\"ShardId\":\"2\",\"State\":\"ACTIVE\"}],\"SupportP2P\":false}";
    const std::string OPEN_AND_GET_OFFSET_RESULT = "{\"Offsets\":{\"0\":{\"Sequence\":1,\"SessionId\":1,\"Timestamp\":1,\"Version\":1},\"1\":{\"Sequence\":1,\"SessionId\":1,\"Timestamp\":1,\"Version\":1},\"2\":{\"Sequence\":1,\"SessionId\":1,\"Timestamp\":1,\"Version\":1}}}";
    const std::string GET_CURSOR_RESULT = "{\"Cursor\":\"200000000001000000000000abcdabcd\",\"RecordTime\":1532411365149,\"Sequence\":1234}";
    const std::string GET_RECORD_RESULT = "{\"NextCursor\":\"200000000001000000000000abcdabce\",\"RecordCount\":10,\"StartSeq\":2,\"Records\":[{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d0145060\",\"Cursor\":\"200000000001000000000000cfac5050\",\"Sequence\":2,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d07c5070\",\"Cursor\":\"200000000001000000000000d0145060\",\"Sequence\":3,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d0e45080\",\"Cursor\":\"200000000001000000000000d07c5070\",\"Sequence\":4,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d14c5090\",\"Cursor\":\"200000000001000000000000d0e45080\",\"Sequence\":5,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d1b450a0\",\"Cursor\":\"200000000001000000000000d14c5090\",\"Sequence\":6,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d21c50b0\",\"Cursor\":\"200000000001000000000000d1b450a0\",\"Sequence\":7,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d28450c0\",\"Cursor\":\"200000000001000000000000d21c50b0\",\"Sequence\":8,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d2ec50d0\",\"Cursor\":\"200000000001000000000000d28450c0\",\"Sequence\":9,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d35450e0\",\"Cursor\":\"200000000001000000000000d2ec50d0\",\"Sequence\":10,\"Data\":[\"1465999335123456\",\"30\"]},{\"SystemTime\":1532411365149,\"NextCursor\":\"200000000001000000000000d3bc50f0\",\"Cursor\":\"200000000001000000000000d35450e0\",\"Sequence\":11,\"Data\":[\"1465999335123456\",\"30\"]}]}";

    const std::string GET_RECORD_RESULT_WITH_ZERO_COUNT = "{\"NextCursor\":\"200000000001000000000000abcdabce\",\"RecordCount\":0}";
    const std::string HEARTBEAT_RESULT_WITH_NULL = "{\"ShardList\":[],\"TotalPlan\":\"test\",\"PlanVersion\":0}";

    const std::string LIST_SHARD_RESULT_1 = "{\"Interval\":300000,\"Protocol\":\"http1.1\",\"Shards\":[{\"BeginHashKey\":\"00000000000000000000000000000000\",\"BeginKey\":\"00000000000000000000000000000000\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"55555555555555555555555555555555\",\"EndKey\":\"55555555555555555555555555555555\",\"LeftShardId\":\"4294967295\",\"ParentShardIds\":[],\"RightShardId\":\"1\",\"ShardId\":\"0\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"55555555555555555555555555555555\",\"BeginKey\":\"55555555555555555555555555555555\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"EndKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"LeftShardId\":\"0\",\"ParentShardIds\":[],\"RightShardId\":\"2\",\"ShardId\":\"1\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"BeginKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"EndKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"LeftShardId\":\"1\",\"ParentShardIds\":[],\"RightShardId\":\"4294967295\",\"ShardId\":\"2\",\"State\":\"ACTIVE\"}],\"SupportP2P\":false}";
    const std::string LIST_SHARD_RESULT_2 = "{\"Interval\":300000,\"Protocol\":\"http1.1\",\"Shards\":[{\"BeginHashKey\":\"00000000000000000000000000000000\",\"BeginKey\":\"00000000000000000000000000000000\",\"ClosedTime\":1669864360,\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"55555555555555555555555555555555\",\"EndKey\":\"55555555555555555555555555555555\",\"ParentShardIds\":[],\"ShardId\":\"0\",\"State\":\"CLOSED\"},{\"BeginHashKey\":\"55555555555555555555555555555555\",\"BeginKey\":\"55555555555555555555555555555555\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"EndKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"LeftShardId\":\"4\",\"ParentShardIds\":[],\"RightShardId\":\"2\",\"ShardId\":\"1\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"BeginKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"EndKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"LeftShardId\":\"1\",\"ParentShardIds\":[],\"RightShardId\":\"4294967295\",\"ShardId\":\"2\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"00000000000000000000000000000000\",\"BeginKey\":\"00000000000000000000000000000000\",\"Cluster\":\"AY62B\",\"CreateTime\":1669864360,\"EndHashKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"EndKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"LeftShardId\":\"4294967295\",\"ParentShardIds\":[\"0\"],\"RightShardId\":\"4\",\"ShardId\":\"3\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"BeginKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"Cluster\":\"AY62B\",\"CreateTime\":1669864360,\"EndHashKey\":\"55555555555555555555555555555555\",\"EndKey\":\"55555555555555555555555555555555\",\"LeftShardId\":\"3\",\"ParentShardIds\":[\"0\"],\"RightShardId\":\"1\",\"ShardId\":\"4\",\"State\":\"ACTIVE\"}],\"SupportP2P\":false}";
    const std::string LIST_SHARD_RESULT_3 = "{\"Interval\":300000,\"Protocol\":\"http1.1\",\"Shards\":[{\"BeginHashKey\":\"00000000000000000000000000000000\",\"BeginKey\":\"00000000000000000000000000000000\",\"ClosedTime\":1669864360,\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"55555555555555555555555555555555\",\"EndKey\":\"55555555555555555555555555555555\",\"ParentShardIds\":[],\"ShardId\":\"0\",\"State\":\"CLOSED\"},{\"BeginHashKey\":\"55555555555555555555555555555555\",\"BeginKey\":\"55555555555555555555555555555555\",\"ClosedTime\":1669864584,\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"EndKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"ParentShardIds\":[],\"ShardId\":\"1\",\"State\":\"CLOSED\"},{\"BeginHashKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"BeginKey\":\"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\",\"ClosedTime\":1669864584,\"Cluster\":\"AY62B\",\"CreateTime\":1669021963,\"EndHashKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"EndKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"ParentShardIds\":[],\"ShardId\":\"2\",\"State\":\"CLOSED\"},{\"BeginHashKey\":\"00000000000000000000000000000000\",\"BeginKey\":\"00000000000000000000000000000000\",\"Cluster\":\"AY62B\",\"CreateTime\":1669864360,\"EndHashKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"EndKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"LeftShardId\":\"4294967295\",\"ParentShardIds\":[\"0\"],\"RightShardId\":\"4\",\"ShardId\":\"3\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"BeginKey\":\"0000000000000000AAAAAAAAAAAAAAAA\",\"Cluster\":\"AY62B\",\"CreateTime\":1669864360,\"EndHashKey\":\"55555555555555555555555555555555\",\"EndKey\":\"55555555555555555555555555555555\",\"LeftShardId\":\"3\",\"ParentShardIds\":[\"0\"],\"RightShardId\":\"5\",\"ShardId\":\"4\",\"State\":\"ACTIVE\"},{\"BeginHashKey\":\"55555555555555555555555555555555\",\"BeginKey\":\"55555555555555555555555555555555\",\"Cluster\":\"AY62B\",\"CreateTime\":1669864584,\"EndHashKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"EndKey\":\"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF\",\"LeftShardId\":\"4\",\"ParentShardIds\":[\"1\",\"2\"],\"RightShardId\":\"4294967295\",\"ShardId\":\"5\",\"State\":\"ACTIVE\"}],\"SupportP2P\":false}";

    JoinGroupResult mJoinGroupResult;
    HeartbeatResult mHeartbeatResult;
    GetTopicResult mGetTopicResult;
    ListShardResult mListShardResult;
    OpenSubscriptionOffsetSessionResult mOpenSubscriptionOffsetSessionResult;
    GetCursorResult mGetCursorResult;
    GetRecordResult mGetRecordResult;

    GetRecordResult mGetRecordResultWithZeroCount;
    HeartbeatResult mHeartbeatResultWithNull;
    ListShardResult mListShardResult1;
    ListShardResult mListShardResult2;
    ListShardResult mListShardResult3;
};

inline GMockResult& GMockResult::GetInstance()
{
    static GMockResult sMockResult;
    return sMockResult;
}

inline GMockResult::GMockResult()
{
    std::vector<DatahubResult*> results{
        &mJoinGroupResult,
        &mHeartbeatResult,
        &mGetTopicResult,
        &mListShardResult,
        &mOpenSubscriptionOffsetSessionResult,
        &mGetCursorResult,
        &mGetRecordResult,
        &mGetRecordResultWithZeroCount,
        &mHeartbeatResultWithNull,
        &mListShardResult1,
        &mListShardResult2,
        &mListShardResult3
    };
    std::vector<std::string> strs{
        JOIN_GROUP_RESULT,
        HEARTBEAT_RESULT,
        GET_TOPIC_RESULT,
        LIST_SHARD_RESULT,
        OPEN_AND_GET_OFFSET_RESULT,
        GET_CURSOR_RESULT,
        GET_RECORD_RESULT,
        GET_RECORD_RESULT_WITH_ZERO_COUNT,
        HEARTBEAT_RESULT_WITH_NULL,
        LIST_SHARD_RESULT_1,
        LIST_SHARD_RESULT_2,
        LIST_SHARD_RESULT_3
    };
    for (size_t i = 0; i < results.size(); i++)
    {
        results[i]->DeserializePayload(strs[i]);
    }
}

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_RESULT_GMOCK_H