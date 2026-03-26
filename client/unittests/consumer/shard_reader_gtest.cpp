#include "timer.h"
#include "logger.h"
#include "offset_meta.h"
#include "message_reader.h"
#include "datahub/datahub_exception.h"
#include "client/consumer/consumer_config.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#define private public
#include "shard_reader.h"
#include "datahub_client_factory.h"

#define ERROR_MESSAGE "GetRecord fail"

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class ShardReaderGTest: public Test
{
protected:
    virtual void SetUp()
    {
        account.id = "testAccessId";
        account.key = "testAccesskey";
        endpoint = "http://successEndpoint";
        projectName = "testProject";
        topicName = "testTopic";
        subId = "1667792277584ABCDE";
        shardId = "0";
        messageReaderPtr = std::make_shared<MessageReader>(2, 100);

        datahubClientPtr = std::make_shared<GMockDatahubClient>(Configuration(account, endpoint));
        std::string key;
        key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;
    }

    virtual void TearDown()
    {
        shardReaderPtr.reset();
        datahubClientPtr.reset();
        messageReaderPtr.reset();
    }

    static GetRecordResult GetRecordWithException(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& cursor,
        int limit,
        const std::string& subId)
    {
        throw DatahubException(LOCAL_ERROR_CODE, ERROR_MESSAGE);
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static std::string subId;
    static std::string shardId;
    static ConsumeOffsetMeta offsetMeta;
    static ShardReaderPtr shardReaderPtr;
    static MessageReaderPtr messageReaderPtr;
    static GMockDatahubClientPtr datahubClientPtr;
};

Account ShardReaderGTest::account;
std::string ShardReaderGTest::endpoint;
std::string ShardReaderGTest::projectName;
std::string ShardReaderGTest::topicName;
std::string ShardReaderGTest::subId;
std::string ShardReaderGTest::shardId;
ConsumeOffsetMeta ShardReaderGTest::offsetMeta;
ShardReaderPtr ShardReaderGTest::shardReaderPtr = nullptr;
MessageReaderPtr ShardReaderGTest::messageReaderPtr = nullptr;
GMockDatahubClientPtr ShardReaderGTest::datahubClientPtr = nullptr;


TEST_F(ShardReaderGTest, GetNextCursorWithNextCursorTest)
{
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->mCurrOffset = ConsumeOffsetMeta();
    shardReaderPtr->mCurrOffset.SetNextCursor("200000000001000000000000abcdabcd");
    std::string getCursor = shardReaderPtr->GetNextCursor();
    ASSERT_EQ(getCursor, "200000000001000000000000abcdabcd");
}

TEST_F(ShardReaderGTest, GetNextCursorWithSequenceTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->mCurrOffset = ConsumeOffsetMeta(0, -1l, -1l, -1l);
    std::string getCursor = shardReaderPtr->GetNextCursor();
    ASSERT_EQ(getCursor, "200000000001000000000000abcdabcd");
}

TEST_F(ShardReaderGTest, GetNextCursorWithTimestampTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->mCurrOffset = ConsumeOffsetMeta(-1l, 1669616552345, -1l, -1l);
    std::string getCursor = shardReaderPtr->GetNextCursor();
    ASSERT_EQ(getCursor, "200000000001000000000000abcdabcd");
}

TEST_F(ShardReaderGTest, GetNextCursorWithOLDESTTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->mCurrOffset = ConsumeOffsetMeta();
    std::string getCursor = shardReaderPtr->GetNextCursor();
    ASSERT_EQ(getCursor, "200000000001000000000000abcdabcd");
}

TEST_F(ShardReaderGTest, GetRecordTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    std::string cursor = "200000000001000000000000abcdabcd";
    GetRecordResult getRecord = shardReaderPtr->GetNextRecordResult(cursor);
    ASSERT_EQ(getRecord.GetNextCursor(), "200000000001000000000000abcdabce");
    ASSERT_EQ(getRecord.GetRecordCount(), 10);
    const auto& records = getRecord.GetRecords();
    for (auto it = records.begin(); it != records.end(); it++)
    {
        ASSERT_EQ(it->GetBigint(0), 1465999335123456l);
        ASSERT_EQ(it->GetInteger(1), 30);
    }
}

TEST_F(ShardReaderGTest, FetchOnceTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->FetchOnce();
    sleep(1);
    auto& fetchQueue = shardReaderPtr->mCompletedFetchCacheQueue;
    ASSERT_EQ(fetchQueue.size(), 10u);
    while (fetchQueue.size() > 0)
    {
        const auto& complete = fetchQueue.front();
        ASSERT_TRUE(complete.mType == T_NORMAL);
        ASSERT_TRUE(complete.mExceptionPtr == nullptr);
        ASSERT_TRUE(complete.mTimerPtr == nullptr);
        ASSERT_TRUE(complete.mRecordPtr != nullptr);
        ASSERT_EQ(complete.mRecordPtr->GetBigint(0), 1465999335123456l);
        ASSERT_EQ(complete.mRecordPtr->GetInteger(1), 30);
        fetchQueue.pop();
    }
}

TEST_F(ShardReaderGTest, FetchOnceWithDelayTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResultWithZeroCount()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->FetchOnce();
    sleep(1);
    auto& fetchQueue = shardReaderPtr->mCompletedFetchCacheQueue;
    ASSERT_EQ(fetchQueue.size(), 1u);
    const auto& complete = fetchQueue.front();
    ASSERT_TRUE(complete.mType == T_DELAY);
    ASSERT_TRUE(complete.mRecordPtr == nullptr);
    ASSERT_TRUE(complete.mExceptionPtr == nullptr);
    ASSERT_TRUE(complete.mTimerPtr != nullptr);
    fetchQueue.pop();
}

TEST_F(ShardReaderGTest, FetchOnceWithDatahubExceptionTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Invoke(ShardReaderGTest::GetRecordWithException));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->FetchOnce();
    sleep(1);
    auto& fetchQueue = shardReaderPtr->mCompletedFetchCacheQueue;
    ASSERT_EQ(fetchQueue.size(), 1u);
    const auto& complete = fetchQueue.front();
    ASSERT_TRUE(complete.mType == T_EXCEPTION);
    ASSERT_TRUE(complete.mRecordPtr == nullptr);
    ASSERT_TRUE(complete.mTimerPtr == nullptr);
    ASSERT_TRUE(complete.mExceptionPtr != nullptr);
    fetchQueue.pop();
}

TEST_F(ShardReaderGTest, SendNextTaskTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->SendNextTask();
    sleep(1);
    auto& fetchQueue = shardReaderPtr->mCompletedFetchCacheQueue;
    ASSERT_EQ(fetchQueue.size(), 10u);
    while (fetchQueue.size() > 0)
    {
        const auto& complete = fetchQueue.front();
        ASSERT_TRUE(complete.mType == T_NORMAL);
        ASSERT_TRUE(complete.mExceptionPtr == nullptr);
        ASSERT_TRUE(complete.mTimerPtr == nullptr);
        ASSERT_TRUE(complete.mRecordPtr != nullptr);
        ASSERT_EQ(complete.mRecordPtr->GetBigint(0), 1465999335123456l);
        ASSERT_EQ(complete.mRecordPtr->GetInteger(1), 30);
        fetchQueue.pop();
    }
}

TEST_F(ShardReaderGTest, SendNextTaskWithDelayTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResultWithZeroCount()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->SendNextTask();
    sleep(1);
    auto& fetchQueue = shardReaderPtr->mCompletedFetchCacheQueue;
    ASSERT_EQ(fetchQueue.size(), 1u);
    const auto& complete = fetchQueue.front();
    ASSERT_TRUE(complete.mType == T_DELAY);
    ASSERT_TRUE(complete.mRecordPtr == nullptr);
    ASSERT_TRUE(complete.mExceptionPtr == nullptr);
    ASSERT_TRUE(complete.mTimerPtr != nullptr);
    fetchQueue.pop();
}

TEST_F(ShardReaderGTest, SendNextTaskWithDatahubExceptionTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Invoke(ShardReaderGTest::GetRecordWithException));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    shardReaderPtr->SendNextTask();
    sleep(1);
    auto& fetchQueue = shardReaderPtr->mCompletedFetchCacheQueue;
    ASSERT_EQ(fetchQueue.size(), 1u);
    const auto& complete = fetchQueue.front();
    ASSERT_TRUE(complete.mType == T_EXCEPTION);
    ASSERT_TRUE(complete.mRecordPtr == nullptr);
    ASSERT_TRUE(complete.mTimerPtr == nullptr);
    ASSERT_TRUE(complete.mExceptionPtr != nullptr);
    fetchQueue.pop();
}

TEST_F(ShardReaderGTest, ReadTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(::testing::AtLeast(1)).
            WillRepeatedly(::testing::Return(GMockResult::GetInstance().GetGetRecordResult()));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    int64_t readNum = 0, targetNum = 12345;
    while (true)
    {
        auto recordPtr = shardReaderPtr->Read(1000);
        if (recordPtr != nullptr)
        {
            ASSERT_EQ(recordPtr->GetBigint(0), 1465999335123456l);
            ASSERT_EQ(recordPtr->GetInteger(1), 30);
            if (++readNum == targetNum)
            {
                std::cout << readNum << std::endl;
                break;
            }
        }
    }
    ASSERT_EQ(shardReaderPtr->mFetchTotalNum.load(), targetNum);
}

TEST_F(ShardReaderGTest, ReadWithDatahubExceptionTest)
{
    EXPECT_CALL(*datahubClientPtr.get(), GetCursor(::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(1).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetCursorResult()));
    EXPECT_CALL(*datahubClientPtr.get(), GetRecord(::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_, ::testing::_)).
            Times(3).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResult())).
            WillOnce(::testing::Return(GMockResult::GetInstance().GetGetRecordResult())).
            WillOnce(::testing::Invoke(ShardReaderGTest::GetRecordWithException));
    ConsumerConfiguration conf(account, endpoint);
    shardReaderPtr = std::make_shared<ShardReader>(projectName, topicName, subId, conf, messageReaderPtr, shardId, offsetMeta);

    int64_t readNum = 0, expectNum = 20l;
    while (true)
    {
        auto recordPtr = shardReaderPtr->Read(1000);
        if (recordPtr != nullptr)
        {
            ASSERT_EQ(recordPtr->GetBigint(0), 1465999335123456l);
            ASSERT_EQ(recordPtr->GetInteger(1), 30);
            if (++readNum == expectNum)
            {
                break;
            }
        }
    }

    try
    {
        auto recordPtr = shardReaderPtr->Read(1000);
        throw std::exception();
    }
    catch (const DatahubException& e)
    {
        ASSERT_EQ(e.GetErrorCode(), LOCAL_ERROR_CODE);
        ASSERT_EQ(e.GetErrorMessage(), ERROR_MESSAGE);
    }
    ASSERT_EQ(shardReaderPtr->mFetchTotalNum.load(), expectNum);
}
