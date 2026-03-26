#include "meta_cache.h"
#include "datahub/datahub_client.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#define private public
#include "datahub_client_factory.h"
#include "record_pack.h"

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class RecordPackGTest : public Test
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
    }

    static Account account;
    static std::string endpoint;
    static std::string projectName;
    static std::string topicName;
    static std::string subId;
};

Account RecordPackGTest::account;
std::string RecordPackGTest::endpoint;
std::string RecordPackGTest::projectName;
std::string RecordPackGTest::topicName;
std::string RecordPackGTest::subId;


TEST_F(RecordPackGTest, AppendReocrdWithoutReadyTest)
{
    size_t recordNum = 1, epochNum = 1;
    int64_t maxBufferSize = 1000;
    int64_t maxBufferRecordCount = 100;
    int64_t maxBufferTimeMs = 1000;
    RecordPack recordPack(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    for (size_t i = 0 ; i < epochNum; i++)
    {
        recordPack.TryAppend(records);
    }
    ASSERT_EQ(recordPack.IsReady(), false);
    ASSERT_NE(recordPack.TryAppend(records), nullptr);
    ASSERT_TRUE(recordPack.mCurrCount <= maxBufferRecordCount);
    ASSERT_TRUE(recordPack.mCurrSize <= maxBufferSize);
}

TEST_F(RecordPackGTest, AppendReocrdWithReadyByTimeoutTest)
{
    size_t recordNum = 1, epochNum = 1;
    int64_t maxBufferSize = 1000;
    int64_t maxBufferRecordCount = 100;
    int64_t maxBufferTimeMs = 1000;
    RecordPack recordPack(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    for (size_t i = 0 ; i < epochNum; i++)
    {
        recordPack.TryAppend(records);
    }

    sleep(1);
    ASSERT_EQ(recordPack.IsReady(), true);
    ASSERT_TRUE(recordPack.mCurrCount <= maxBufferRecordCount);
    ASSERT_TRUE(recordPack.mCurrSize <= maxBufferSize);
}

TEST_F(RecordPackGTest, AppendReocrdWithReadyByBuffferSizeTest)
{
    size_t recordNum = 1, epochNum = 50;
    int64_t maxBufferSize = 1000;
    int64_t maxBufferRecordCount = 1000;
    int64_t maxBufferTimeMs = 1000;
    RecordPack recordPack(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    for (size_t i = 0 ; i < epochNum; i++)
    {
        recordPack.TryAppend(records);
    }

    ASSERT_EQ(recordPack.IsReady(), false);

    recordPack.TryAppend(records);
    ASSERT_EQ(recordPack.IsReady(), true);
    ASSERT_TRUE(recordPack.mCurrCount <= maxBufferRecordCount);
    ASSERT_TRUE(recordPack.mCurrSize <= maxBufferSize);
}

TEST_F(RecordPackGTest, AppendReocrdWithReadyByBuffferCountTest)
{
    size_t recordNum = 1, epochNum = 100;
    int64_t maxBufferSize = 10000;
    int64_t maxBufferRecordCount = 100;
    int64_t maxBufferTimeMs = 1000;
    RecordPack recordPack(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    for (size_t i = 0 ; i < epochNum; i++)
    {
        recordPack.TryAppend(records);
    }

    ASSERT_EQ(recordPack.IsReady(), false);

    recordPack.TryAppend(records);
    ASSERT_EQ(recordPack.IsReady(), true);
    ASSERT_TRUE(recordPack.mCurrCount <= maxBufferRecordCount);
    ASSERT_TRUE(recordPack.mCurrSize <= maxBufferSize);
}