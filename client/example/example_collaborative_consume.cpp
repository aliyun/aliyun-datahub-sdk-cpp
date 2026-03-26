#include <cstring>
#include <iostream>
#include <thread>
#include <atomic>
#include <stdlib.h>
#include <getopt.h>
#include "client/consumer/consumer_config.h"
#include "client/consumer/datahub_consumer.h"
#include "datahub/datahub_record.h"
#include <functional>

#define PROG_VERSION "1.0"

using namespace aliyun;
using namespace aliyun::datahub;

std::atomic_int gTotalNum(0);

Account gAccount;
std::string gEndpoint;
std::string gProjectName;
std::string gTopicName;
std::string gSubId;
bool gAutoAck;
int32_t gFetchNum;
int64_t gSessionTimeoutMs;

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
        "   -s --subId              <string>          set subscription id [Mandatory]\n"
        "   -a --autoAck            <bool>            set auto ack offset\n"
        "   -f --fetchNum           <int>             set fetch limit num\n"
        "   -S --sessionTimeoutMs   <int>             set session timeout (ms)\n"
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
        { "subId", 1, 0, 's' },
        { "autoAck", 1, 0, 'a'},
        { "fetchNum", 1, 0, 'f'},
        { "sessionTimeoutMs", 1, 0, 'S'},
        { "version", 0, 0, 'v' },
        { "help", 0, 0, 'h' },
        { NULL, 0, 0, 0 }
    };

    while (1)
    {
        int ret = getopt_long(argc, argv, "i:k:e:p:t:s:a:f:S:vh", long_opts, &opt_id);
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
        case 's':
            gSubId = optarg;
            break;
        case 'a':
            gAutoAck = (bool)std::atoi(optarg);
            break;
        case 'f':
            gFetchNum = std::atoi(optarg);
            break;
        case 'S':
            gSessionTimeoutMs = (int64_t)std::atoi(optarg);
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
}

inline void ProcessRecords(const TopicMetaPtr& topicMeta, const RecordResultPtr& recordPtr)
{
    const RecordSchema& schema = topicMeta->GetRecordSchema();
    if (schema.GetFieldCount() == 0)        // BLOB
    {
        int len = 0;
        const char* data = recordPtr->GetData(len);
        printf("%s\n", data);
    }
    else                                    // TUPLE
    {
        for (int j = 0; j < schema.GetFieldCount(); ++j)
        {
            const Field& field = schema.GetField(j);
            const FieldType fieldType = field.GetFieldType();
            switch (fieldType)
            {
                case BIGINT:
                    printf("%ld ", recordPtr->GetBigint(j));
                    break;
                case INTEGER:
                    printf("%d ", recordPtr->GetInteger(j));
                    break;
                case SMALLINT:
                    printf("%hd ", recordPtr->GetSmallint(j));
                    break;
                case TINYINT:
                    printf("%hhd ", recordPtr->GetTinyint(j));
                    break;
                case DOUBLE:
                    printf("%.5lf ", recordPtr->GetDouble(j));
                    break;
                case FLOAT:
                    printf("%.5f ", recordPtr->GetFloat(j));
                    break;
                case STRING:
                    printf("%s ", recordPtr->GetString(j).c_str());
                    break;
                case BOOLEAN:
                    printf("%s ", recordPtr->GetBoolean(j) ? "true" : "false");
                    break;
                case TIMESTAMP:
                    printf("%ld ", recordPtr->GetTimestamp(j));
                    break;
                case DECIMAL:
                    printf("%s ", recordPtr->GetDecimal(j).c_str());
                    break;
                default:
                    break;
            }
        }
        printf("\n");
    }
}

void CollaborativeConsume(const std::string& project, const std::string& topic, const std::string& subId, const ConsumerConfiguration& conf)
{
    DatahubConsumer consumer(project, topic, subId, conf);
    const TopicMetaPtr& topicMeta = consumer.GetTopicMeta();

    uint64_t readRecordNum = 0;
    try{
        while (true)
        {
            auto recordPtr = consumer.Read(60000);
            if (recordPtr == nullptr)
            {
                break;
            }
            // TODO: 处理数据
            ProcessRecords(topicMeta, recordPtr);
            if (!gAutoAck)
            {
                recordPtr->GetMessageKey()->Ack();      // 如果auto_ack设置为false,则处理完数据执行Ack;
            }
            readRecordNum++;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Read fail: " << e.what() << std::endl;
    }
    std::cout << "Read " << readRecordNum << " records total" << std::endl;
}

int main(int argc, char* argv[])
{
    parse_args(argc, argv);

    ConsumerConfiguration consumerConf(gAccount, gEndpoint);
    consumerConf.SetLogFilePath("./DatahubCollaborativeConsumer.log");
    consumerConf.SetAutoAckOffset(gAutoAck);
    if (gFetchNum > 0)
    {
        consumerConf.SetFetchLimitNum(gFetchNum);
    }
    if (gSessionTimeoutMs > 0)
    {
        consumerConf.SetSessionTimeout(gSessionTimeoutMs);
    }

    CollaborativeConsume(gProjectName, gTopicName, gSubId, consumerConf);

    return 0;
}
