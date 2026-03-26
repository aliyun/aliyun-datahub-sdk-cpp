#include <cstring>
#include <iostream>
#include <getopt.h>
#include <functional>
#include "client/write_result.h"
#include "client/producer/producer_config.h"
#include "client/producer/datahub_producer.h"
#include "datahub/datahub_record.h"

#define PROG_VERSION "1.0"

using namespace aliyun;
using namespace aliyun::datahub;

Account gAccount;
std::string gEndpoint;
std::string gProjectName;
std::string gTopicName;
int gEpochNum = 10;
int gRecordNum = 10;
int64_t gMaxBufferRecords = 0l;
int64_t gMaxBufferSize = 0l;
int64_t gMaxBufferTimeMs = 0l;
int64_t gMaxRecordPackQueueLimit = 0l;
StringVec gShardIds;

inline void StringSplit(StringVec& tar, const std::string& src, const std::string& pre)
{
    size_t start_pos = 0, end_pos = src.size();
    size_t pos = src.find(pre, start_pos);
    while (pos != std::string::npos)
    {
        std::string addStr = src.substr(start_pos, pos - start_pos);
        if (!addStr.empty())
        {
            tar.push_back(addStr);
        }
        start_pos = pos + 1;
        pos = src.find(pre, start_pos);
    }
    std::string addStr = src.substr(start_pos, end_pos - start_pos);
    if (!addStr.empty())
    {
        tar.push_back(addStr);
    }
}

void Usage(const char* prog)
{
    const char *start = strrchr(prog, '/');
    start = (start != NULL) ? start + 1 : prog;
    fprintf(stdout,
        "%s Ver %s\n"
        "Usage: <programme> [options...]\n"
        "   -i --accessid           <string>          set access id [Mandatory]\n"
        "   -k --accesskey          <string>          set access key [Mandatory]\n"
        "   -e --endpoint           <string>          set endpoint [Mandatory]\n"
        "   -p --project            <string>          set project [Mandatory]\n"
        "   -t --topic              <string>          set topic [Mandatory]\n"
        "   -E --epochNum           <int>             set epoch num for write [Mandatory]\n"
        "   -N --recordNum          <int>             set record num for each epoch [Mandatory]\n"
        "   -R --maxBufferRecords   <int>             set max buffer record count\n"
        "   -S --maxBufferSize      <int>             set max buffer size\n"
        "   -M --maxBufferTimeMs    <int>             set max buffer time (ms)\n"
        "   -Q --maxRecordPackLimit <int>             set max record pack queue limit\n"
        "   -H --shardIds           <string>          set shards (split by ',')\n"
        "   -v --version                              show version\n"
        "   -h --help                                 show help message\n", start, PROG_VERSION);
}

void parse_args(int argc, char* argv[])
{
    int opt_id;
    static struct option long_opts[] =
    {
        { "accessid", 1, 0, 'i'},
        { "accesskey", 1, 0, 'k'},
        { "endpoint", 1, 0, 'e' },
        { "project", 1, 0, 'p' },
        { "topic", 1, 0, 't' },
        { "epochNum", 1, 0, 'E'},
        { "recordNum", 1, 0, 'N'},
        { "maxBufferRecords", 1, 0, 'R'},
        { "mMaxBufferSize", 1, 0, 'S'},
        { "maxBufferTimeMs", 1, 0, 'M'},
        { "maxRecordPackQueueLimit", 1, 0, 'Q'},
        { "shardIds", 1, 0, 'H'},
        { "version", 0, 0, 'v' },
        { "help", 0, 0, 'h' },
        { NULL, 0, 0, 0 }
    };

    while (1)
    {
        int ret = getopt_long(argc, argv, "i:k:e:p:t:E:N:R:S:M:Q:H:vh", long_opts, &opt_id);
        if (ret == -1)
        {
            break;
        }
        switch (ret)
        {
        case 'i':
            gAccount.id = optarg;
            break;
        case 'k':
            gAccount.key = optarg;
            break;
        case 'e':
            gEndpoint = optarg;
            break;
        case 'p':
            gProjectName = optarg;
            break;
        case 't':
            gTopicName = optarg;
            break;
        case 'E':
            gEpochNum = std::atoi(optarg);
            break;
        case 'N':
            gRecordNum = std::atoi(optarg);
            break;
        case 'R':
            gMaxBufferRecords = (int64_t)std::atoi(optarg);
            break;
        case 'S':
            gMaxBufferSize = (int64_t)std::atoi(optarg);
            break;
        case 'M':
            gMaxBufferTimeMs = (int64_t)std::atoi(optarg);
            break;
        case 'Q':
            gMaxRecordPackQueueLimit = (int64_t)std::atoi(optarg);
            break;
        case 'H':
            StringSplit(gShardIds, optarg, ",");
            break;
        case 'v':
        case 'h':
            Usage(argv[0]);
            exit(0);
            break;
        default:
            break;
        }
    }

    if (gEpochNum <= 0 || gRecordNum <= 0)
    {
        std::cerr << "Invalid parameter!" << std::endl;
        exit(1);
    }
}

inline void GenerateTupleRecord(const RecordSchema& schema, RecordEntry& record)
{
    for (int i = 0; i < schema.GetFieldCount(); i++)
    {
        const Field& field = schema.GetField(i);
        const FieldType& type = field.GetFieldType();
        switch (type)
        {
            case BIGINT:
                record.SetBigint(i, 1234l);
                break;
            case DOUBLE:
                record.SetDouble(i, 1.234);
                break;
            case BOOLEAN:
                record.SetBoolean(i, true);
                break;
            case TIMESTAMP:
                record.SetTimestamp(i, 1234l);
                break;
            case STRING:
                record.SetString(i, "1234");
                break;
            case DECIMAL:
                record.SetDecimal(i, "1234");
                break;
            case INTEGER:
                record.SetInteger(i, (int32_t)1234);
                break;
            case FLOAT:
                record.SetFloat(i, 1.234);
                break;
            case TINYINT:
                record.SetTinyint(i, (int8_t)1234);
                break;
            case SMALLINT:
                record.SetSmallint(i, (int16_t)1234);
                break;
            default:
                break;
        }
    }
}

inline void GenerateRecords(TopicMetaPtr topicMeta, RecordEntryVec& records)
{
    const RecordSchema& schema = topicMeta->GetRecordSchema();
    for (int i = 0; i < gRecordNum; i++)
    {
        if (topicMeta->GetRecordType() == "TUPLE")
        {
            // TUPLE
            records.emplace_back(schema.GetFieldCount());
            GenerateTupleRecord(schema, records.back());
        }
        else
        {
            // BLOB
            records.emplace_back(BLOB);
            records.back().SetData("test_blob_data" + std::to_string(i));
        }
    }
}

void AsyncProduce(const std::string& project, const std::string& topic, const ProducerConfiguration& conf, const StringVec& shardIds)
{
    DatahubProducer producer(project, topic, conf, shardIds);
    const TopicMetaPtr& topicMeta = producer.GetTopicMeta();

    std::vector<WriteResultFuturePtr> resultFutureVecs;
    std::map<std::string, uint64_t> writeRecordNum;
    for (const std::string& shardId : shardIds)
    {
        writeRecordNum[shardId] = 0;
    }

    try
    {
        // TODO: 生成数据
        RecordEntryVec records;
        GenerateRecords(topicMeta, records);
        for (int i = 0; i < gEpochNum; i++)
        {
            auto result = producer.WriteAsync(records);
            resultFutureVecs.push_back(result);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "WriteAsync fail: " << e.what() << std::endl;
    }

    producer.Flush();

    for (auto it = resultFutureVecs.begin(); it != resultFutureVecs.end(); it++)
    {
        (*it)->wait();
        try
        {
            WriteResultPtr result = (*it)->get();
            writeRecordNum[result->GetShardId()] += gRecordNum;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Write records fail: " << e.what() << std::endl;
        }
    }

    for (auto it = writeRecordNum.begin(); it != writeRecordNum.end(); it++)
    {
        std::cout << "Write " << it->second << " records to shard " << it->first << std::endl;
    }
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    ProducerConfiguration producerConf(gAccount, gEndpoint);
    producerConf.SetEnableProtobuf(true);
    producerConf.SetLogFilePath("./DatahubAsyncProducer.log");
    if (gMaxBufferRecords > 0)
    {
        producerConf.SetMaxAsyncBufferRecords(gMaxBufferRecords);
    }
    if (gMaxBufferSize > 0)
    {
        producerConf.SetMaxAsyncBufferSize(gMaxBufferSize);
    }
    if (gMaxBufferTimeMs > 0)
    {
        producerConf.SetMaxAsyncBufferTimeMs(gMaxBufferTimeMs);
    }
    if (gMaxRecordPackQueueLimit > 0)
    {
        producerConf.SetMaxRecordPackQueueLimit(gMaxRecordPackQueueLimit);
    }

    AsyncProduce(gProjectName, gTopicName, producerConf, gShardIds);

    return 0;
}
