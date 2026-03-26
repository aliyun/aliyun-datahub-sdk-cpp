#include <algorithm>
#include <unistd.h>
#include "datahub_client_json_impl.h"
#include "http/curl/curl_http_client.h"
#include "http/http_request.h"
#include "utils.h"
#include "error_parser.h"
#include "datahub/datahub_compressor.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_result.h"

namespace aliyun
{
namespace datahub
{

DatahubClientJsonImpl::DatahubClientJsonImpl(const Configuration& conf) :
    mUseV4Sign(conf.IsA4Signing()),
    mCompressLevel(conf.GetCompressLevel()),
    mCompress(conf.GetCompress()),
    mHttpClient(new http::CurlHttpClient(conf.GetMaxPoolSize(), conf.GetRequestTimeout(), conf.GetConnectTimeout())),
    mEndpoint(conf.GetEndpoint()),
    mContentType(http::CONTENT_TYPE_JSON),
    mUserAgent(Utils::GenUserAgent(conf.GetUserInfo(), SDK_VERSION)),
    mVersion(CLIENT_VERSION),
    mSigner(conf.GetAccount().id, conf.GetAccount().key, conf.GetAccount().token, conf.GetRegion())
{
    size_t pos = mEndpoint.size();
    while (pos > 0 && mEndpoint[pos-1] == '/') pos--;
    mEndpoint.resize(pos);
    mCompressLevel = std::max(0, mCompressLevel);
}

DatahubClientJsonImpl::~DatahubClientJsonImpl()
{
}

void DatahubClientJsonImpl::AddCommonHeaders(http::HttpRequest& httpRequest) const
{
    httpRequest.SetContentType(mContentType);
    httpRequest.SetUserAgent(mUserAgent);
    httpRequest.SetClientVersion(mVersion);
    httpRequest.SetDate(Utils::GetDate());
}

void DatahubClientJsonImpl::AddHeadersToRequest(
        const std::shared_ptr<http::HttpRequest>& httpRequest,
        const StringMap& headerValues) const
{
    if (!headerValues.empty())
    {
        std::map<std::string, std::string>::const_iterator iter;
        for (iter = headerValues.begin(); iter != headerValues.end(); ++iter) {
            httpRequest->SetHeaderValue(iter->first, iter->second);
        }
    }
}

void DatahubClientJsonImpl::AddContentBodyToRequest(
        const std::shared_ptr<http::HttpRequest>& httpRequest,
        const std::string& body, uint32_t compressMode) const
{
    std::string ss;
    compress::Compressor* compressor = compress::CompressorFactory::GetCompressor(mCompress);
    if ((compressMode & ALLOW_SEND_COMPRESS) && body.size() > 0 && compressor != NULL)
    {
        const std::string& compressedBody = compressor->Compress(body.c_str(), body.size(), mCompressLevel);
        httpRequest->AddContentBody(compressedBody);
        ss.append(std::to_string(compressedBody.size()));

        std::string ss_body(std::to_string(body.size()));

        const std::string& cname = compress::GetNameOfCompressMethod(mCompress);
        httpRequest->SetHeaderValue(http::RAW_SIZE_HEADER, ss_body);
        httpRequest->SetHeaderValue(http::CONTENT_ENCODING, cname);

        ATOMIC_ADD(mWriteMetric.rawBytes, body.size());
        ATOMIC_ADD(mWriteMetric.transBytes, compressedBody.size());
    }
    else
    {
        httpRequest->AddContentBody(body);
        ss.append(std::to_string(body.size()));

        ATOMIC_ADD(mWriteMetric.rawBytes, body.size());
        ATOMIC_ADD(mWriteMetric.transBytes, body.size());
    }

    if (compressor != NULL && (compressMode & ALLOW_RECV_COMPRESS))
    {
        const std::string& cname = compress::GetNameOfCompressMethod(mCompress);
        httpRequest->SetHeaderValue(http::ACCEPT_ENCODING, cname);
    }

    httpRequest->SetContentLength(ss);
}

void DatahubClientJsonImpl::BuildHttpRequest(const DatahubRequest& request,
        const std::shared_ptr<http::HttpRequest>& httpRequest) const
{
    AddCommonHeaders(*httpRequest);

    AddHeadersToRequest(httpRequest, request.GetHeaders());

    AddContentBodyToRequest(httpRequest, request.GetBody(), request.GetCompressMode());

    httpRequest->GetUri().SetPath(request.GetPath());
}

std::shared_ptr<http::HttpResponse> DatahubClientJsonImpl::AttemptOneRequest(
        const DatahubRequest& request, const http::HttpMethod& mehtod)
{
    http::URI uri(mEndpoint);

    std::shared_ptr<http::HttpRequest> httpRequest(new http::HttpRequest(uri, mehtod));

    BuildHttpRequest(request, httpRequest);
    mSigner.SignRequest(*httpRequest, mUseV4Sign);
    Outcome outcome = mHttpClient->MakeRequest(*httpRequest);

    if (!outcome.IsSuccess())
    {
        throw DatahubException(LOCAL_ERROR_CODE, outcome.GetError());
    }

    return outcome.GetHttpResponse();
}

void DatahubClientJsonImpl::ParseResponse(const std::shared_ptr<http::HttpResponse>& response, DatahubResult& result)
{
    if (response == NULL)
    {
        throw DatahubException(LOCAL_ERROR_CODE, "Http connection fail");
    }

    compress::CompressMethod compressMethod = compress::GetCompressMethodByName(response->GetHeader(http::CONTENT_ENCODING));
    compress::Compressor* compressor = compress::CompressorFactory::GetCompressor(compressMethod);

    size_t realBodySize = response->GetResponseBody()->size();
    if (compressor != NULL)
    {
        const std::string& rawSizeStr = response->GetHeader(http::RAW_SIZE_HEADER);
        int32_t rawSize = rawSizeStr.empty() ? 0 : atoi(rawSizeStr.c_str());

        const std::string& decompressedBody = compressor->Decompress(
            response->GetResponseBody()->c_str(),
            response->GetResponseBody()->size(),
            rawSize);
        response->SetResponseBody(std::make_shared<std::string>(decompressedBody));
    }

    if (IsResponseOK(*response))
    {
        ATOMIC_ADD(mReadMetric.transBytes, realBodySize);
        ATOMIC_ADD(mReadMetric.rawBytes, response->GetResponseBody()->size());

        result.SetRequestId(response->GetHeader(http::REQUEST_ID_HEADER));
        result.DeserializePayload(*response->GetResponseBody());
    }
    else
    {
        throw ErrorParser::Parse(*response);
    }
}

ListProjectResult DatahubClientJsonImpl::ListProject()
{
    ListProjectRequest request;

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    ListProjectResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::CreateProject(const std::string& project, const std::string& comment)
{
    CreateProjectRequest request(project, comment);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    CreateProjectResult result;

    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::DeleteProject(const std::string& project)
{
    DeleteProjectRequest request(project);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_DELETE);

    DeleteProjectResult result;

    ParseResponse(httpResponse, result);
}

GetProjectResult DatahubClientJsonImpl::GetProject(const std::string& project)
{
    GetProjectRequest request(project);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    GetProjectResult result(project);
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::UpdateProject(const std::string& project, const std::string& comment)
{
    UpdateProjectRequest request(project, comment);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    UpdateProjectResult result;

    ParseResponse(httpResponse, result);
}

ListTopicResult DatahubClientJsonImpl::ListTopic(const std::string& project)
{
    ListTopicRequest request(project);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    ListTopicResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::CreateTopic(
    const std::string& project,
    const std::string& topic,
    int shardCount,
    int lifeCycle,
    const RecordType& type,
    const RecordSchema& schema,
    const std::string& comment)
{
    CreateTopicRequest request(project, topic, shardCount, lifeCycle, type, schema, comment);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    CreateTopicResult result;

    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::CreateTopic(
    const std::string& project,
    const std::string& topic,
    int shardCount,
    int lifeCycle,
    const RecordType& type,
    const std::string& comment)
{
    CreateTopicRequest request(project, topic, shardCount, lifeCycle, type, comment);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    CreateTopicResult result;

    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::DeleteTopic(const std::string& project, const std::string& topic)
{
    DeleteTopicRequest request(project, topic);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_DELETE);

    DeleteTopicResult result;

    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::UpdateTopic(
        const std::string& project,
        const std::string& topic,
        int lifeCycle,
        const std::string& comment)
{
    UpdateTopicRequest request(project, topic, lifeCycle, comment);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    UpdateTopicResult result;

    ParseResponse(httpResponse, result);
}

GetTopicResult DatahubClientJsonImpl::GetTopic(const std::string& project, const std::string& topic)
{
    GetTopicRequest request(project, topic);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    GetTopicResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::AppendField(
        const std::string& project,
        const std::string& topic,
        const std::string& fieldName,
        const std::string& fieldType)
{
    AppendFieldRequest request(project, topic, fieldName, fieldType);

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    AppendFieldResult result;

    ParseResponse(httpResponse, result);
}

ListShardResult DatahubClientJsonImpl::ListShard(
        const std::string& project, const std::string& topic)
{
    ListShardRequest request(project, topic);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    ListShardResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::ActivateShard(
        const std::string& project,
        const std::string& topic,
        const StringVec& shards)
{
    ActivateShardRequest request(project, topic, shards);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    ActivateShardResult result;
    ParseResponse(httpResponse, result);
}

SplitShardResult DatahubClientJsonImpl::SplitShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& splitKey)
{
    SplitShardRequest request(project, topic, shardId, splitKey);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    SplitShardResult result;
    ParseResponse(httpResponse, result);

    return result;
}

MergeShardResult DatahubClientJsonImpl::MergeShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& adjacentShardId)
{
    MergeShardRequest request(project, topic, shardId, adjacentShardId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    MergeShardResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::WaitForShardReady(
        const std::string& project,
        const std::string& topic,
        uint64_t timeout)
{
    uint64_t now = time(NULL);
    uint64_t end = now + timeout;
    while (now < end)
    {
        if (IsShardLoadCompleted(project, topic))
        {
            return;
        }
        sleep(2);
        now = time(NULL);
    }
    if (!IsShardLoadCompleted(project, topic))
    {
        throw DatahubException(SERVICE_ERROR_CODE, "Shard is opening or closing");
    }
}

GetMeteringInfoResult DatahubClientJsonImpl::GetMeteringInfo(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId)
{
    GetMeteringInfoRequest request(project, topic, shardId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetMeteringInfoResult result;

    ParseResponse(httpResponse, result);

    return result;
}

GetTopicMeteringInfoResult DatahubClientJsonImpl::GetTopicMeteringInfo(
        const std::string& project,
        const std::string& topic,
        const std::string& day)
{
    GetTopicMeteringInfoRequest request(project, topic, day);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetTopicMeteringInfoResult result;

    ParseResponse(httpResponse, result);

    return result;
}

PutRecordResult DatahubClientJsonImpl::PutRecord(const std::string& project,
        const std::string& topic,
        const std::vector<RecordEntry>& records)
{
    ATOMIC_ADD(mWriteMetric.requests, 1lu);
    ATOMIC_ADD(mWriteMetric.records, records.size());

    PutRecordRequest request(project, topic, records);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    PutRecordResult result;
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

PutRecordByShardResult DatahubClientJsonImpl::PutRecordByShard(const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::vector<RecordEntry>& records)
{
    throw DatahubException(LOCAL_ERROR_CODE, "Put shard by id only support pb mode");
}

GetCursorResult DatahubClientJsonImpl::GetCursor(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const CursorType& type)
{
    GetCursorRequest request(project, topic, shardId, type);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetCursorResult result;
    ParseResponse(httpResponse, result);

    return result;
}

GetCursorResult DatahubClientJsonImpl::GetCursor(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const CursorType& type,
        int64_t value)
{
    GetCursorRequest request(project, topic, shardId, value, type);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetCursorResult result;
    ParseResponse(httpResponse, result);

    return result;
}

GetRecordResult DatahubClientJsonImpl::GetRecord(
        const std::string& project,
        const std::string& topic,
        const std::string& shardId,
        const std::string& cursor,
        int limit,
        const std::string& subId)
{
    ATOMIC_ADD(mReadMetric.requests, 1lu);
    GetRecordRequest request(project, topic, shardId, cursor, limit);
    if (!subId.empty())
    {
        request.SetSubId(subId);
    }

    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);
    GetRecordResult result;
    ParseResponse(httpResponse, result);
    ATOMIC_ADD(mReadMetric.records, (uint64_t)result.GetRecordCount());

    return result;
}

ListConnectorResult DatahubClientJsonImpl::ListConnector(
        const std::string& project,
        const std::string& topic)
{
    ListConnectorRequest request(project, topic);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    ListConnectorResult result;
    ParseResponse(httpResponse, result);

    return result;
}

CreateConnectorResult DatahubClientJsonImpl::CreateConnector(
        const std::string& project,
        const std::string& topic,
        const sdk::ConnectorType& type,
        int64_t sinkStartTime,
        const StringVec& columnFields,
        const sdk::SinkConfig& config)
{
    CreateConnectorRequest request(project, topic, type, sinkStartTime, columnFields, config);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    CreateConnectorResult result;
    ParseResponse(httpResponse, result);

    return result;
}

CreateConnectorResult DatahubClientJsonImpl::CreateConnector(
        const std::string& project,
        const std::string& topic,
        const sdk::ConnectorType& type,
        const StringVec& columnFields,
        const sdk::SinkConfig& config)
{
    CreateConnectorRequest request(project, topic, type, columnFields, config);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    CreateConnectorResult result;
    ParseResponse(httpResponse, result);

    return result;
}

GetConnectorResult DatahubClientJsonImpl::GetConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    GetConnectorRequest request(project, topic, connectorId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    GetConnectorResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::UpdateConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::SinkConfig& config)
{
    UpdateConnectorRequest request(project, topic, connectorId, config);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    UpdateConnectorResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::DeleteConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    DeleteConnectorRequest request(project, topic, connectorId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_DELETE);

    DeleteConnectorResult result;
    ParseResponse(httpResponse, result);
}

GetConnectorDoneTimeResult DatahubClientJsonImpl::GetConnectorDoneTime(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    GetConnectorDoneTimeRequest request(project, topic, connectorId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    GetConnectorDoneTimeResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::ReloadConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    ReloadConnectorRequest request(project, topic, connectorId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    ReloadConnectorResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::ReloadConnector(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId)
{
    ReloadConnectorRequest request(project, topic, connectorId, shardId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    ReloadConnectorResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::UpdateConnectorState(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::ConnectorState& state)
{
    UpdateConnectorStateRequest request(project, topic, connectorId, state);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    UpdateConnectorStateResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::UpdateConnectorOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const sdk::ConnectorOffset& connectorOffset)
{
    UpdateConnectorOffsetRequest request(project, topic, connectorId, connectorOffset);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    UpdateConnectorOffsetResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::UpdateConnectorOffsetByShard(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId,
        const sdk::ConnectorOffset& connectorOffset)
{
    UpdateConnectorOffsetRequest request(project, topic, connectorId, connectorOffset);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    UpdateConnectorOffsetResult result;
    ParseResponse(httpResponse, result);
}

GetConnectorShardStatusResult DatahubClientJsonImpl::GetConnectorShardStatus(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId)
{
    GetConnectorShardStatusRequest request(project, topic, connectorId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetConnectorShardStatusResult result;
    ParseResponse(httpResponse, result);

    return result;
}

GetConnectorShardStatusByShardResult DatahubClientJsonImpl::GetConnectorShardStatusByShard(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& shardId)
{
    GetConnectorShardStatusByShardRequest request(project, topic, connectorId, shardId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetConnectorShardStatusByShardResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::AppendConnectorField(
        const std::string& project,
        const std::string& topic,
        const std::string& connectorId,
        const std::string& fieldName)
{
    AppendConnectorFieldRequest request(project, topic, connectorId, fieldName);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    AppendConnectorFieldResult result;
    ParseResponse(httpResponse, result);
}

ListSubscriptionResult DatahubClientJsonImpl::ListSubscription(
        const std::string& project,
        const std::string& topic,
        int pageIndex,
        int pageSize,
        const std::string& search)
{
    ListSubscriptionRequest request(project, topic, pageIndex, pageSize, search);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    ListSubscriptionResult result;
    ParseResponse(httpResponse, result);

    return result;
}

ListSubscriptionResult DatahubClientJsonImpl::ListSubscription(
        const std::string& project,
        const std::string& topic,
        int pageIndex,
        int pageSize)
{
    ListSubscriptionRequest request(project, topic, pageIndex, pageSize);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    ListSubscriptionResult result;
    ParseResponse(httpResponse, result);

    return result;
}

CreateSubscriptionResult DatahubClientJsonImpl::CreateSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& comment)
{
    CreateSubscriptionRequest request(project, topic, comment);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    CreateSubscriptionResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::DeleteSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& subId)
{
    DeleteSubscriptionRequest request(project, topic, subId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_DELETE);

    DeleteSubscriptionResult result;
    ParseResponse(httpResponse, result);
}

GetSubscriptionResult DatahubClientJsonImpl::GetSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& subId)
{
    GetSubscriptionRequest request(project, topic, subId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_GET);

    GetSubscriptionResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::UpdateSubscription(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::string& comment)
{
    UpdateSubscriptionRequest request(project, topic, subId, comment);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    UpdateSubscriptionResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::UpdateSubscriptionState(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const SubscriptionState& state)
{
    UpdateSubscriptionStateRequest request(project, topic, subId, state);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    UpdateSubscriptionStateResult result;
    ParseResponse(httpResponse, result);
}

OpenSubscriptionOffsetSessionResult DatahubClientJsonImpl::InitSubscriptionOffsetSession(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const StringVec& shardIds)
{
    OpenSubscriptionOffsetSessionRequest request(project, topic, subId, shardIds);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    OpenSubscriptionOffsetSessionResult result;
    ParseResponse(httpResponse, result);

    return result;
}

GetSubscriptionOffsetResult DatahubClientJsonImpl::GetSubscriptionOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const StringVec& shardIds)
{
    GetSubscriptionOffsetRequest request(project, topic, subId, shardIds);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetSubscriptionOffsetResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::UpdateSubscriptionOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::map<std::string, SubscriptionOffset>& offsets)
{
    UpdateSubscriptionOffsetRequest request(project, topic, subId, offsets);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    UpdateSubscriptionOffsetResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::ResetSubscriptionOffset(
        const std::string& project,
        const std::string& topic,
        const std::string& subId,
        const std::map<std::string, SubscriptionOffset>& offsets)
{
    ResetSubscriptionOffsetRequest request(project, topic, subId, offsets);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    ResetSubscriptionOffsetResult result;
    ParseResponse(httpResponse, result);
}


HeartbeatResult DatahubClientJsonImpl::Heartbeat(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId,
        const StringVec& holdShardList,
        const StringVec& readEndShardList)
{
    HeartbeatRequest request(project, topic, consumerGroup, consumerId, versionId, holdShardList, readEndShardList);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    HeartbeatResult result;
    ParseResponse(httpResponse, result);

    return result;
}

JoinGroupResult DatahubClientJsonImpl::JoinGroup(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        int64_t sessionTimeout)
{
    JoinGroupRequest request(project, topic, consumerGroup, sessionTimeout);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    JoinGroupResult result;
    ParseResponse(httpResponse, result);

    return result;
}

void DatahubClientJsonImpl::SyncGroup(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId,
        const StringVec& releaseShardList,
        const StringVec& readEndShardList)
{
    SyncGroupRequest request(project, topic, consumerGroup, consumerId, versionId, releaseShardList, readEndShardList);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    SyncGroupResult result;
    ParseResponse(httpResponse, result);
}

void DatahubClientJsonImpl::LeaveGroup(
        const std::string& project,
        const std::string& topic,
        const std::string& consumerGroup,
        const std::string& consumerId,
        int64_t versionId)
{
    LeaveGroupRequest request(project, topic, consumerGroup, consumerId, versionId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_PUT);

    LeaveGroupResult result;
    ParseResponse(httpResponse, result);
}

RegisterTopicSchemaResult DatahubClientJsonImpl::RegisterTopicSchema(
    const std::string& project,
    const std::string& topic,
    const RecordSchema& schema)
{
    RegisterTopicSchemaRequest request(project, topic, schema);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    RegisterTopicSchemaResult result;
    ParseResponse(httpResponse, result);

    return result;
}

GetTopicSchemaResult DatahubClientJsonImpl::GetTopicSchema(
    const std::string& project,
    const std::string& topic,
    int64_t versionId,
    const RecordSchema& schema)
{
    GetTopicSchemaRequest request(project, topic, versionId, schema);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    GetTopicSchemaResult result;
    ParseResponse(httpResponse, result);

    return result;
}

ListTopicSchemaResult DatahubClientJsonImpl::ListTopicSchema(
    const std::string& project,
    const std::string& topic,
    int pageNumber,
    int pageSize)
{
    ListTopicSchemaRequest request(project, topic, pageNumber, pageSize);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    ListTopicSchemaResult result;
    ParseResponse(httpResponse, result);

    return result;
}

DeleteTopicSchemaResult DatahubClientJsonImpl::DeleteTopicSchema(
    const std::string& project,
    const std::string& topic,
    int64_t versionId)
{
    DeleteTopicSchemaRequest request(project, topic, versionId);
    std::shared_ptr<http::HttpResponse> httpResponse = AttemptOneRequest(request, http::HTTP_POST);

    DeleteTopicSchemaResult result;
    ParseResponse(httpResponse, result);

    return result;
}

bool DatahubClientJsonImpl::IsShardLoadCompleted(const std::string& project, const std::string& topic)
{
    try
    {
        const ListShardResult& ret = ListShard(project, topic);
        const std::vector<ShardEntry>& shards = ret.GetShards();
        for (size_t i = 0; i < shards.size(); ++i)
        {
            if (shards[i].GetState() != ACTIVE && shards[i].GetState() != CLOSED)
            {
                return false;
            }
        }
        return true;
    }
    catch(DatahubException& e)
    {
    }
    return false;
}

} // namespace datahub
} // namespace aliyun
