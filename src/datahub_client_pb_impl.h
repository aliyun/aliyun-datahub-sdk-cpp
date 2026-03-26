#ifndef DATAHUB_SDK_DATAHUB_CLIENT_PB_IMPL_H
#define DATAHUB_SDK_DATAHUB_CLIENT_PB_IMPL_H

#include <string>
#include <memory>
#include "datahub/datahub_config.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_request.h"
#include "datahub/datahub_result.h"
#include "datahub_client_json_impl.h"

namespace aliyun
{
namespace datahub
{

class DatahubClientPBImpl : public DatahubClientJsonImpl
{
public:
    explicit DatahubClientPBImpl(const Configuration& conf);
    ~DatahubClientPBImpl();

    PutRecordResult PutRecord(
            const std::string& project,
            const std::string& topic,
            const std::vector<RecordEntry>& records);

    PutRecordByShardResult PutRecordByShard(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::vector<RecordEntry>& records);

    GetRecordResult GetRecord(
            const std::string& project,
            const std::string& topic,
            const std::string& shardId,
            const std::string& cursor,
            int limit = 10,
            const std::string& subId = "");
};

} // namespace datahub
} // namespace aliyun
#endif
