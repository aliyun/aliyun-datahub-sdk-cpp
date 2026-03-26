#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <exception>
#include <stdlib.h>
#include "gtest/gtest.h"
#include "datahub/datahub_exception.h"
#include "config.h"
#include "datahub/datahub_client.h"

using namespace aliyun;
using namespace aliyun::datahub;

static const int SHARDCOUNT = 3;
static const int LIFECYCLE = 7;
static const std::string COMMENT = "test_comment";
compress::CompressMethod compressType = compress::NONE;
static std::string bytes;

class PutRecordByShardGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        Configuration conf(account, conf_endpoint, true);
        conf.SetCompress(compressType);
        m_client = new DatahubClient(conf);

        bytes.resize(256);
        for (int i = 0; i < 256; ++i) {
            bytes[i] = i - 128;
        }

        conf.SetCompress(compress::LZ4);
        m_client_lz4 = new DatahubClient(conf);

        conf.SetCompress(compress::ZLIB);
        m_client_zlib = new DatahubClient(conf);

        conf.SetCompress(compress::DEFLATE);
        m_client_deflate = new DatahubClient(conf);

        m_schema.AddField(Field("a", STRING));
        m_schema.AddField(Field("b", BIGINT));
        m_schema.AddField(Field("c", DOUBLE));
        m_schema.AddField(Field("d", TIMESTAMP));
        m_schema.AddField(Field("e", BOOLEAN));
        m_schema.AddField(Field("f", DECIMAL));
        m_schema.AddField(Field("g", INTEGER));
        m_schema.AddField(Field("h", FLOAT));
        m_schema.AddField(Field("i", TINYINT));
        m_schema.AddField(Field("j", SMALLINT));

        m_project = conf_project;
        srand((unsigned)time(NULL));
        std::stringstream ss;
        ss << rand();
        m_topic = "test_topic_" + ss.str();
        m_blob_topic = "test_blob_topic_" + ss.str();

        try
        {
            m_client->CreateProject(m_project, "project for topic gtest");
        }
        catch (std::exception& e)
        {
        }

        try
        {
            m_client->CreateTopic(m_project, m_topic, SHARDCOUNT, LIFECYCLE, TUPLE, m_schema, COMMENT);
            m_client->WaitForShardReady(m_project, m_topic);
        }
        catch (std::exception& e)
        {
        }
        try
        {
            m_client->CreateTopic(m_project, m_blob_topic, SHARDCOUNT, LIFECYCLE, BLOB, COMMENT);
            m_client->WaitForShardReady(m_project, m_blob_topic);
        }
        catch (std::exception& e)
        {
        }

    }

    virtual void TearDown()
    {
        try
        {
            m_client->DeleteTopic(m_project, m_blob_topic);
        }
        catch (std::exception& e)
        {
        }
        try
        {
            m_client->DeleteTopic(m_project, m_topic);
        }
        catch (std::exception& e)
        {
        }

        try
        {
            m_client->DeleteProject(m_project);
        }
        catch (std::exception& e)
        {
        }

        delete m_client;
        delete m_client_lz4;
        delete m_client_zlib;
        delete m_client_deflate;
        compressType = compress::NONE;
    }

    static RecordEntry GenErrorRecordEntry(const RecordSchema& schema)
    {
        RecordEntry entry(schema.GetFieldCount());
        entry.SetString(schema.GetFieldIndex("a"), "string");
        entry.SetBigint(schema.GetFieldIndex("b"), 5L);
        entry.SetDouble(schema.GetFieldIndex("c"), 0.0);
        entry.SetTimestamp(schema.GetFieldIndex("d"), 123456789000000L);
        entry.SetBoolean(schema.GetFieldIndex("e"), true);
        entry.SetDecimal(schema.GetFieldIndex("f"), "123.123");
        entry.SetInteger(schema.GetFieldIndex("g"), 123);
        entry.SetFloat(schema.GetFieldIndex("h"), 456.789);
        entry.SetTinyint(schema.GetFieldIndex("i"), 1);
        entry.SetSmallint(schema.GetFieldIndex("j"), 2);
        return entry;
    }

    static RecordEntry GenRecordEntry(const RecordSchema& schema)
    {
        RecordEntry entry(schema.GetFieldCount());
        entry.SetString(schema.GetFieldIndex("a"), "string");
        entry.SetBigint(schema.GetFieldIndex("b"), 5L);
        entry.SetDouble(schema.GetFieldIndex("c"), 0.0);
        entry.SetTimestamp(schema.GetFieldIndex("d"), 123456789000000L);
        entry.SetBoolean(schema.GetFieldIndex("e"), true);
        entry.SetDecimal(schema.GetFieldIndex("f"), "123.123");
        entry.SetInteger(schema.GetFieldIndex("g"), 123);
        entry.SetFloat(schema.GetFieldIndex("h"), 456.789);
        entry.SetTinyint(schema.GetFieldIndex("i"), 1);
        entry.SetSmallint(schema.GetFieldIndex("j"), 2);
        entry.SetAttribute("partition", "ds=2016");
        return entry;
    }

   static RecordEntry GenRecordEntryNull(const RecordSchema& schema)
    {
        RecordEntry entry(schema.GetFieldCount());
        entry.SetAttribute("partition", "ds=2016");
        return entry;
    }

    static DatahubClient* m_client;
    static DatahubClient* m_client_lz4;
    static DatahubClient* m_client_zlib;
    static DatahubClient* m_client_deflate;
    static std::string m_project;
    static std::string m_topic;
    static std::string m_blob_topic;
    static RecordSchema m_schema;
};

DatahubClient* PutRecordByShardGTest::m_client;
DatahubClient* PutRecordByShardGTest::m_client_lz4;
DatahubClient* PutRecordByShardGTest::m_client_zlib;
DatahubClient* PutRecordByShardGTest::m_client_deflate;
std::string PutRecordByShardGTest::m_project;
std::string PutRecordByShardGTest::m_topic;
std::string PutRecordByShardGTest::m_blob_topic;
RecordSchema PutRecordByShardGTest::m_schema;

TEST_F(PutRecordByShardGTest, PutRecordTest)
{
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_topic);
    RecordSchema schema = getTopicResult.GetRecordSchema();
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry = GenRecordEntry(schema);
        std::stringstream ss;
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client->PutRecordByShard(m_project, m_topic, "0", recordEntries));

    sleep(3);
    /* Get Cursor */
    GetCursorResult r1 = m_client->GetCursor(m_project, m_topic, "0", CURSOR_TYPE_OLDEST);
    std::string cursor = r1.GetCursor();
    ASSERT_EQ(r1.GetTimestamp() >= 0, true);
    ASSERT_EQ(r1.GetSequence() >= 0, true);

    /* Get Record */
    GetRecordResult r2 = m_client->GetRecord(m_project, m_topic, "0", cursor, recordNumber);
    int count = r2.GetRecordCount();
    ASSERT_NE(count, 0);
    for (int i = 0; i < count; ++i)
    {
        const RecordResult& recordResult = r2.GetRecord(i);
        ASSERT_NE(recordResult.GetCursor(), "");
        ASSERT_NE(recordResult.GetSystemTime(), -1);
        ASSERT_NE(recordResult.GetSequence(), -1);
        ASSERT_EQ(recordResult.GetString(0), "string");
        ASSERT_EQ(recordResult.GetBigint(1), 5L);
        ASSERT_EQ(recordResult.GetDouble(2), 0.0);
        ASSERT_EQ(recordResult.GetTimestamp(3), 123456789000000L);
        ASSERT_EQ(recordResult.GetBoolean(4), true);
        ASSERT_EQ(recordResult.GetDecimal(5), "123.123");
        ASSERT_EQ(recordResult.GetInteger(6), 123);
        ASSERT_TRUE(std::abs(recordResult.GetFloat(7) - 456.789) < 0.00001);
        ASSERT_EQ(recordResult.GetTinyint(8), 1);
        ASSERT_EQ(recordResult.GetSmallint(9), 2);
    }
}

TEST_F(PutRecordByShardGTest, PutRecordFailedTest)
{
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_topic);
    RecordSchema schema = getTopicResult.GetRecordSchema();
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry = GenRecordEntry(schema);
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client->PutRecordByShard(m_project, m_topic, "0", recordEntries));

    sleep(5);
    m_client->MergeShard(m_project, m_topic, "0", "1");
    m_client->WaitForShardReady(m_project, m_topic);

    try
    {
        m_client->PutRecordByShard(m_project, m_topic, "0", recordEntries);
    }
    catch (DatahubException& e)
    {
        ASSERT_EQ(e.GetErrorMessage(), "The specified shard is not active.");
    }
}

TEST_F(PutRecordByShardGTest, PutBlobRecordTest)
{
    sleep(5);
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_blob_topic);
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry(BLOB);
        std::stringstream ss;
        ss << i;
        const std::string& shardId = "0";
        entry.SetData(ss.str().c_str(), shardId.size());
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client->PutRecordByShard(m_project, m_blob_topic, "0", recordEntries));

    sleep(5);
    /* Get Cursor */
    GetCursorResult r1 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_OLDEST);
    std::string cursor = r1.GetCursor();
    ASSERT_EQ(r1.GetTimestamp() >= 0, true);
    ASSERT_EQ(r1.GetSequence() >= 0, true);

    /* Get Record */
    GetRecordResult r2 = m_client->GetRecord(m_project, m_blob_topic, "0", cursor, recordNumber);
    int count = r2.GetRecordCount();
    ASSERT_EQ(recordNumber, count);
    for (int i = 0; i < count; ++i)
    {
        std::stringstream ss;
        ss << i;

        const RecordResult& recordResult = r2.GetRecord(i);
        int len = 0;
        const char * data = recordResult.GetData(len);
        ASSERT_EQ(std::string(data, len), ss.str());
    }

    /* Get Cursor by TIESTAMP */
    GetCursorResult r3 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_TIMESTAMP, r1.GetTimestamp());
    ASSERT_EQ(r3.GetTimestamp() >= 0, true);
    ASSERT_EQ(r3.GetSequence() >= 0, true);

    /* Get Cursor by SEQUENCE */
    int64_t seq = count / 2;
    GetCursorResult r4 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_SEQUENCE, seq);
    ASSERT_EQ(r4.GetTimestamp(), r1.GetTimestamp());
    ASSERT_EQ(r4.GetSequence(), seq);
}

TEST_F(PutRecordByShardGTest, PutNullRecordTest)
{
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_topic);
    RecordSchema schema = getTopicResult.GetRecordSchema();
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry = GenRecordEntryNull(schema);
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client->PutRecordByShard(m_project, m_topic, "0", recordEntries));
}

TEST_F(PutRecordByShardGTest, PutRecordLZ4Test)
{
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_topic);
    RecordSchema schema = getTopicResult.GetRecordSchema();
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry = GenRecordEntry(schema);
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client_lz4->PutRecordByShard(m_project, m_topic, "0", recordEntries));
}

TEST_F(PutRecordByShardGTest, PutBlobRecordLZ4Test)
{
    sleep(5);
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_blob_topic);
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry(BLOB);
        std::stringstream ss;
        ss << i;
        const std::string& shardId = "0";
        entry.SetData(ss.str().c_str(), shardId.size());
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client_lz4->PutRecordByShard(m_project, m_blob_topic, "0", recordEntries));

    sleep(5);
/* Get Cursor */
    GetCursorResult r1 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_OLDEST);
    std::string cursor = r1.GetCursor();
    ASSERT_EQ(r1.GetTimestamp() >= 0, true);
    ASSERT_EQ(r1.GetSequence() >= 0, true);

    /* Get Record */
    GetRecordResult r2 = m_client->GetRecord(m_project, m_blob_topic, "0", cursor, recordNumber);
    int count = r2.GetRecordCount();
    ASSERT_EQ(recordNumber, count);
    for (int i = 0; i < count; ++i)
    {
        std::stringstream ss;
        ss << i;

        const RecordResult& recordResult = r2.GetRecord(i);
        int len = 0;
        const char * data = recordResult.GetData(len);
        ASSERT_EQ(std::string(data, len), ss.str());
    }

    /* Get Cursor by TIESTAMP */
    GetCursorResult r3 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_TIMESTAMP, r1.GetTimestamp());
    ASSERT_EQ(r3.GetTimestamp() >= 0, true);
    ASSERT_EQ(r3.GetSequence() >= 0, true);

    /* Get Cursor by SEQUENCE */
    int64_t seq = count / 2;
    GetCursorResult r4 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_SEQUENCE, seq);
    ASSERT_EQ(r4.GetTimestamp(), r1.GetTimestamp());
    ASSERT_EQ(r4.GetSequence(), seq);
}

TEST_F(PutRecordByShardGTest, PutRecordZLIBTest)
{
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_topic);
    RecordSchema schema = getTopicResult.GetRecordSchema();
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry = GenRecordEntry(schema);
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client_zlib->PutRecordByShard(m_project, m_topic, "0", recordEntries));
}

TEST_F(PutRecordByShardGTest, PutBlobRecordZLIBTest)
{
    sleep(5);
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_blob_topic);
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry(BLOB);
        std::stringstream ss;
        ss << i;
        const std::string& shardId = "0";
        entry.SetData(ss.str().c_str(), shardId.size());
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client_zlib->PutRecordByShard(m_project, m_blob_topic, "0", recordEntries));

    sleep(5);
/* Get Cursor */
    GetCursorResult r1 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_OLDEST);
    std::string cursor = r1.GetCursor();
    ASSERT_EQ(r1.GetTimestamp() >= 0, true);
    ASSERT_EQ(r1.GetSequence() >= 0, true);

    /* Get Record */
    GetRecordResult r2 = m_client->GetRecord(m_project, m_blob_topic, "0", cursor, recordNumber);
    int count = r2.GetRecordCount();
    ASSERT_EQ(recordNumber, count);
    for (int i = 0; i < count; ++i)
    {
        std::stringstream ss;
        ss << i;

        const RecordResult& recordResult = r2.GetRecord(i);
        int len = 0;
        const char * data = recordResult.GetData(len);
        ASSERT_EQ(std::string(data, len), ss.str());
    }

    /* Get Cursor by TIESTAMP */
    GetCursorResult r3 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_TIMESTAMP, r1.GetTimestamp());
    ASSERT_EQ(r3.GetTimestamp() >= 0, true);
    ASSERT_EQ(r3.GetSequence() >= 0, true);

    /* Get Cursor by SEQUENCE */
    int64_t seq = count / 2;
    GetCursorResult r4 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_SEQUENCE, seq);
    ASSERT_EQ(r4.GetTimestamp(), r1.GetTimestamp());
    ASSERT_EQ(r4.GetSequence(), seq);
}

TEST_F(PutRecordByShardGTest, PutRecordDeflateTest)
{
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_topic);
    RecordSchema schema = getTopicResult.GetRecordSchema();
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry = GenRecordEntry(schema);
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client_deflate->PutRecordByShard(m_project, m_topic, "0", recordEntries));
}

TEST_F(PutRecordByShardGTest, PutBlobRecordDeflateTest)
{
    sleep(5);
    GetTopicResult getTopicResult = m_client->GetTopic(m_project, m_blob_topic);
    std::vector<RecordEntry> recordEntries;
    int recordNumber = 10;
    for (int i = 0; i < recordNumber; ++i)
    {
        RecordEntry entry(BLOB);
        std::stringstream ss;
        ss << i;
        const std::string& shardId = "0";
        entry.SetData(ss.str().c_str(), shardId.size());
        entry.SetAttribute("partition", "ds=2016");
        recordEntries.push_back(entry);
    }
    EXPECT_NO_THROW(m_client_deflate->PutRecordByShard(m_project, m_blob_topic, "0", recordEntries));

    sleep(5);
/* Get Cursor */
    GetCursorResult r1 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_OLDEST);
    std::string cursor = r1.GetCursor();
    ASSERT_EQ(r1.GetTimestamp() >= 0, true);
    ASSERT_EQ(r1.GetSequence() >= 0, true);

    /* Get Record */
    GetRecordResult r2 = m_client->GetRecord(m_project, m_blob_topic, "0", cursor, recordNumber);
    int count = r2.GetRecordCount();
    ASSERT_EQ(recordNumber, count);
    for (int i = 0; i < count; ++i)
    {
        std::stringstream ss;
        ss << i;

        const RecordResult& recordResult = r2.GetRecord(i);
        int len = 0;
        const char * data = recordResult.GetData(len);
        ASSERT_EQ(std::string(data, len), ss.str());
    }

    /* Get Cursor by TIESTAMP */
    GetCursorResult r3 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_TIMESTAMP, r1.GetTimestamp());
    ASSERT_EQ(r3.GetTimestamp() >= 0, true);
    ASSERT_EQ(r3.GetSequence() >= 0, true);

    /* Get Cursor by SEQUENCE */
    int64_t seq = count / 2;
    GetCursorResult r4 = m_client->GetCursor(m_project, m_blob_topic, "0", CURSOR_TYPE_SEQUENCE, seq);
    ASSERT_EQ(r4.GetTimestamp(), r1.GetTimestamp());
    ASSERT_EQ(r4.GetSequence(), seq);
}

