#include <unistd.h>
#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <unistd.h>
#include "datahub/datahub_client.h"

using namespace aliyun;
using namespace aliyun::datahub;

int main(int argc, char *argv[])
{
    /* Configuration */
    Account account;
    account.id = "";
    account.key = "=";

    std::string projectName = "test_project";
    std::string topicName = "test_cpp";
    std::string comment = "test";

    std::string endpoint = "";
    Configuration conf(account, endpoint);

    /* Datahub Client */
    DatahubClient client(conf);

    /* Create Project */
    try
    {
        client.CreateProject(projectName, comment);
    }
    catch(std::exception& e)
    {
    }

    /* Create Topic */
    int shardCount = 3;
    int lifeCycle = 7;
    RecordType type = BLOB;
    try
    {
        client.CreateTopic(projectName, topicName, shardCount, lifeCycle, type, comment);
    }
    catch(std::exception& e)
    {
    }

    client.WaitForShardReady(projectName, topicName); 

    /* Build Record */
    std::vector<RecordEntry> records;
    RecordEntry record(type);
    record.SetShardId("0");
    std::string msg = "testbllobea";
    record.SetData(msg.c_str(), msg.size());
    records.push_back(record);

    /* Put Record */
    PutRecordResult r0 = client.PutRecord(projectName, topicName, records);

    /* Get Cursor */
    GetCursorResult r1 = client.GetCursor(projectName, topicName, "0", CURSOR_TYPE_OLDEST);

    std::string cursor = r1.GetCursor();

    /* Get Record */
    GetRecordResult r2 = client.GetRecord(projectName, topicName, "0", cursor, 10);
    int count = r2.GetRecordCount();
    std::cout << "read record count is: " << count << std::endl;
    for (int i = 0; i < count; ++i)
    {
        const RecordResult& recordResult = r2.GetRecord(i);
        int len = 0;
        const char * data = recordResult.GetData(len);
        std::cout<<std::string(data, len)<<std::endl;
    }
    return 0;
}
