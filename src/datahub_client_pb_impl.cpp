#include <string>
#include <memory>
#include <iostream>

#include "datahub/datahub_request.h"
#include "datahub/datahub_result.h"
#include "datahub_client_pb_impl.h"

namespace aliyun
{
namespace datahub
{

DatahubClientPBImpl::DatahubClientPBImpl(const Configuration& conf) :
    DatahubClientJsonImpl(conf)
{
}

DatahubClientPBImpl::~DatahubClientPBImpl()
{
}

PutRecordResult DatahubClientPBImpl::PutRecord(const std::string& project,
        const std::string& topic,
        const std::vector<RecordEntry>& records)
{
    ATOMIC_ADD(mWriteMetric.requests, 1lu);
    ATOMIC_ADD(mWriteMetric.records, records.size());

    PutPBRecordRequest request(project, topic, records);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    PutPBRecordResult result;
    ParseResponse(httpResponse, result);

    if (result.GetFailedRecordCount() > 0)
    {
        std::vector<RecordEntry> failedRecords;
        for (const auto& entry : result.GetErrorEntries())
        {
            failedRecords.push_back(records[entry.GetIndex()]);
        }
        result.SetFailedRecords(std::move(failedRecords));
    }

    return result;
}

PutRecordByShardResult DatahubClientPBImpl::PutRecordByShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::vector<RecordEntry>& records)
{
    ATOMIC_ADD(mWriteMetric.requests, 1lu);
    ATOMIC_ADD(mWriteMetric.records, records.size());

    PutRecordByShardRequest request(project, topic, shardId, records);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    PutRecordByShardResult result;
    ParseResponse(httpResponse, result);
    return result;
}

GetRecordResult DatahubClientPBImpl::GetRecord(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& cursor,
        int limit,
        const std::string& subId)
{
    ATOMIC_ADD(mReadMetric.requests, 1lu);
    GetPBRecordRequest request(project, topic, shardId, cursor, limit);
    if (!subId.empty())
    {
        request.SetSubId(subId);
    }

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);
    GetPBRecordResult result;
    ParseResponse(httpResponse, result);
    ATOMIC_ADD(mReadMetric.records, (uint64_t)result.GetRecordCount());

    return result;
}

} // namespace datahub
} // namespace aliyun
