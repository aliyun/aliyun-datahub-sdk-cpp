#include "meta_cache.h"
#include "datahub/datahub_client.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "gmock/datahub_client_gmock.h"
#define private public
#include "datahub_client_factory.h"
#include "record_pack_queue.h"

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class RecordPackQueueGTest : public Test
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

Account RecordPackQueueGTest::account;
std::string RecordPackQueueGTest::endpoint;
std::string RecordPackQueueGTest::projectName;
std::string RecordPackQueueGTest::topicName;
std::string RecordPackQueueGTest::subId;


TEST_F(RecordPackQueueGTest, AppendReocrdWithOnceTest)
{
    size_t recordNum = 1;
    int64_t maxBufferSize = 1000;
    int64_t maxBufferRecordCount = 100;
    int64_t maxBufferTimeMs = 1000;
    int32_t maxRecordPackQueueLimit = 128;
    RecordPackQueue recordPackQueue(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs, maxRecordPackQueueLimit);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }

    recordPackQueue.AppendRecord(records);

    ASSERT_EQ(recordPackQueue.mReadyRecordPacks.size(), 0u);
    ASSERT_TRUE(recordPackQueue.mCurrentRecordPack != nullptr);
}

TEST_F(RecordPackQueueGTest, AppendReocrdWithReadyByTimeoutTest)
{
    size_t recordNum = 1;
    int64_t maxBufferSize = 1000;
    int64_t maxBufferRecordCount = 100;
    int64_t maxBufferTimeMs = 1000;
    int32_t maxRecordPackQueueLimit = 128;
    RecordPackQueue recordPackQueue(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs, maxRecordPackQueueLimit);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }

    recordPackQueue.AppendRecord(records);

    ASSERT_EQ(recordPackQueue.mReadyRecordPacks.size(), 0u);
    ASSERT_TRUE(recordPackQueue.mCurrentRecordPack != nullptr);

    sleep(1);
    recordPackQueue.AppendRecord(records);

    ASSERT_EQ(recordPackQueue.mReadyRecordPacks.size(), 1u);
    ASSERT_TRUE(recordPackQueue.mCurrentRecordPack == nullptr);
}

TEST_F(RecordPackQueueGTest, AppendReocrdWithReadyByBuffferSizeTest)
{
    size_t recordNum = 1, epochNum = 370;
    int64_t maxBufferSize = 1000;
    int64_t maxBufferRecordCount = 1000;
    int64_t maxBufferTimeMs = 10000;
    int32_t maxRecordPackQueueLimit = 128;
    RecordPackQueue recordPackQueue(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs, maxRecordPackQueueLimit);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    for (size_t i = 0 ; i < epochNum; i++)
    {
        recordPackQueue.AppendRecord(records);
    }

    uint32_t expectSize = (epochNum * 20 - 1) / maxBufferSize;
    ASSERT_EQ(recordPackQueue.mReadyRecordPacks.size(), expectSize);

    RecordPackPtr readyRecord = nullptr;
    for (uint32_t i = 0; i < expectSize; i++)
    {
        readyRecord = recordPackQueue.ObtainReadyRecordPack(false);
        ASSERT_TRUE(readyRecord);
        recordPackQueue.PopBackRecordPack();
    }

    // Obtain without force
    readyRecord = recordPackQueue.ObtainReadyRecordPack(false);
    ASSERT_FALSE(readyRecord);

    // Obtain with force
    readyRecord = recordPackQueue.ObtainReadyRecordPack(true);
    ASSERT_TRUE(readyRecord);
}

TEST_F(RecordPackQueueGTest, AppendReocrdWithReadyByBuffferCountTest)
{
    size_t recordNum = 1, epochNum = 370;
    int64_t maxBufferSize = 10000;
    int64_t maxBufferRecordCount = 100;
    int64_t maxBufferTimeMs = 10000;
    int32_t maxRecordPackQueueLimit = 128;
    RecordPackQueue recordPackQueue(maxBufferSize, maxBufferRecordCount, maxBufferTimeMs, maxRecordPackQueueLimit);

    RecordEntryVec records;
    for (size_t i = 0; i < recordNum; i++)
    {
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("blob_data_unit_tests");
        records.push_back(record);
    }
    for (size_t i = 0 ; i < epochNum; i++)
    {
        recordPackQueue.AppendRecord(records);
    }

    uint32_t expectSize = (epochNum - 1) / maxBufferRecordCount;
    ASSERT_EQ(recordPackQueue.mReadyRecordPacks.size(), expectSize);

    RecordPackPtr readyRecord = nullptr;
    for (uint32_t i = 0; i < expectSize; i++)
    {
        readyRecord = recordPackQueue.ObtainReadyRecordPack(false);
        ASSERT_TRUE(readyRecord);
        recordPackQueue.PopBackRecordPack();
    }

    // Obtain without force
    readyRecord = recordPackQueue.ObtainReadyRecordPack(false);
    ASSERT_FALSE(readyRecord);

    // Obtain with force
    readyRecord = recordPackQueue.ObtainReadyRecordPack(true);
    ASSERT_TRUE(readyRecord);
}