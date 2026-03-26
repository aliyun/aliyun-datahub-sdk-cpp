#ifndef DATAHUB_SDK_PROTOBUF_TOOL_H
#define DATAHUB_SDK_PROTOBUF_TOOL_H

#include <string>
#include <memory>
#include "datahub/datahub_record.h"
#include "datahub/datahub_result.h"
#include "proto/common_request.pb.h"

namespace aliyun
{
namespace datahub
{

class ProtobufTool
{
public:
    static void MessageToRecordResult(const aliyun::datahub::proto::RecordEntry& pbEntry, RecordResult& entry);
    static void RecordEntryToMessage(const RecordEntry& entry, aliyun::datahub::proto::RecordEntry* pbEntry);
    static void MessageToErrorEntry(const aliyun::datahub::proto::FailedRecord& failedRecord, ErrorEntry& entry);
};

} // namespace datahub
} // namespace aliyun
#endif
