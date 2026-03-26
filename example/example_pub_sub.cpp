#include <iostream>
#include <unistd.h>
#include "datahub/datahub_client.h"
#include "datahub/datahub_config.h"
#include "datahub/datahub_exception.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_record.h"

using namespace std;
using namespace aliyun::datahub;

std::string projectName = "";
std::string topicName = "";
std::string subId = "";
RecordSchema schema;

inline void GenerateRecords(RecordEntryVec& records)
{
    for (int i = 0; i < 10; i++)
    {
        // TUPLE
        // RecordEntry record = RecordEntry(5);
        // record.SetString(0, "field_1_" + std::to_string(i));
        // record.SetBigint(1, 123456789l);
        // record.SetDouble(2, 123.456d);
        // record.SetDouble(3, 654.32100d);
        // record.SetString(4, "field_2_" + std::to_string(i));
        // records.push_back(record);

        // BLOB
        RecordEntry record = RecordEntry(BLOB);
        record.SetData("test_blob_data" + std::to_string(i));
        records.push_back(record);
    }
}

inline void ProcessRecords(const RecordResult& record)
{
    if (schema.GetFieldCount() == 0)       // BLOB
    {
        int len = 0;
        const char* data = record.GetData(len);
        printf("%s\n", data);
    }
    else                                   // TUPLE
    {
        for (int j = 0; j < schema.GetFieldCount(); ++j)
        {
            const Field &field = schema.GetField(j);
            const FieldType fieldType = field.GetFieldType();
            switch (fieldType)
            {
                case BIGINT:
                    printf("%ld\n", record.GetBigint(j));
                    break;
                case DOUBLE:
                    printf("%.15lf\n", record.GetDouble(j));
                    break;
                case STRING:
                    printf("%s\n", record.GetString(j).c_str());
                    break;
                default:
                    break;
            }
        }
    }
}

void pubRetry(DatahubClient& client, const std::vector<RecordEntry>& records, int retryTimes)
{
    if (retryTimes <= 0)
    {
        std::cout << "Put record retry fail. records size: " << records.size() << std::endl;
        return ;
    }

    try
    {
        PutRecordResult prr = client.PutRecord(projectName, topicName, records);
        if (prr.GetFailedRecordCount() > 0)
        {
            pubRetry(client, prr.GetFailedRecords(), retryTimes-1);
        }
        else
        {
            std::cout << "Put records retry success." << std::endl;
        }
    }
    catch (const DatahubException& e)
    {
        std::cerr << "Put records fail when put retry. requestId: " << e.GetRequestId() << ", ErrorCode: " << e.GetErrorCode() << ", ErrorMessage: " << e.GetErrorMessage() << std::endl;
    }
}

void pub(DatahubClient& client)
{
    std::vector<RecordEntry> records;
    GenerateRecords(records);

    try
    {
        PutRecordResult prr = client.PutRecord(projectName, topicName, records);
        if (prr.GetFailedRecordCount() > 0)
        {
            pubRetry(client, prr.GetFailedRecords(), 3);
        }
    }
    catch (const DatahubException& e)
    {
        std::cerr << "Put records fail. requestId: " << e.GetRequestId() << ", ErrorCode: " << e.GetErrorCode() << ", ErrorMessage: " << e.GetErrorMessage() << std::endl;
    }
}

void sub(DatahubClient& client)
{
    std::string shardId = "0";
    std::string cursor;
    OpenSubscriptionOffsetSessionResult osor = client.InitSubscriptionOffsetSession(projectName, topicName, subId, {shardId});
    SubscriptionOffset subscription = osor.GetOffsets().at(shardId);
    if (subscription.GetSequence() < 0)
    {
        cursor = client.GetCursor(projectName, topicName, shardId, CURSOR_TYPE_OLDEST).GetCursor();
    }
    else
    {
        int64_t nextSequence = subscription.GetSequence() + 1;
        try
        {
            cursor = client.GetCursor(projectName, topicName, shardId, CURSOR_TYPE_SEQUENCE, nextSequence).GetCursor();
        }
        catch (const DatahubException& e)
        {
            cursor = client.GetCursor(projectName, topicName, shardId, CURSOR_TYPE_OLDEST).GetCursor();
        }
    }

    int64_t readTotal = 0;
    int fetchNum = 10;
    while (true)
    {
        try
        {
            GetRecordResult grr = client.GetRecord(projectName, topicName, shardId, cursor, fetchNum, subId);
            if (grr.GetRecordCount() <= 0)
            {
                std::cout << "Read null, wait for 1s." << std::endl;
                sleep(1);
                continue;
            }
            
            for (auto recordResult : grr.GetRecords())
            {
                ProcessRecords(recordResult);
                if (++readTotal % 1000 == 0)
                {
                    subscription.SetSequence(recordResult.GetSequence());
                    subscription.SetTimestamp(recordResult.GetSystemTime());
                    std::map<std::string, SubscriptionOffset> offsets;
                    offsets[shardId] = subscription;
                    try
                    {
                        client.UpdateSubscriptionOffset(projectName, topicName, subId, offsets);
                    }
                    catch (const DatahubException& e)
                    {
                        std::cerr << "Update subscription offset fail. requestId: "<< e.GetRequestId() << ", ErrorCode: " << e.GetErrorCode() << ", ErrorMessage: " << e.GetErrorMessage() << std::endl;
                        throw ;
                    }
                }
            }
            cursor = grr.GetNextCursor();
        }
        catch (const DatahubException& e)
        {
            std::cerr << "Get record fail. requestId: " << e.GetRequestId() << ", ErrorCode: " << e.GetErrorCode() << ", ErrorMessage: " << e.GetErrorMessage() << std::endl;
        }
    }
}

int main()
{
     /* Configuration */
    Account account;
    account.id = "";
    account.key = "";

    std::string endpoint = "";
    Configuration conf(account, endpoint);

    /* Datahub Client */
    DatahubClient datahubClient(conf);

    pub(datahubClient);
    sub(datahubClient);

    return 0;
}