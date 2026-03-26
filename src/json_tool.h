#ifndef DATAHUB_SDK_JSON_TOOL_H
#define DATAHUB_SDK_JSON_TOOL_H

#include <string>
#include "rapidjson/document.h"
#include "datahub/datahub_shard.h"
#include "datahub/datahub_record.h"
#include "datahub/datahub_result.h"
#include "datahub/datahub_meter.h"

namespace aliyun
{
namespace datahub
{

class JsonTool
{
public:
    static void JsonToErrorEntry(const rapidjson::Value& jsonValue, ErrorEntry& entry);
    static void JsonToRecordResult(const rapidjson::Value& jsonValue, RecordResult& entry);
    static void RecordEntryToJson(const RecordEntry& entry, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& value);
    static void JsonToShardDesc(const rapidjson::Value& jsonValue, ShardDesc& desc);
    static void JsonToShardEntry(const rapidjson::Value& jsonValue, ShardEntry& entry);
    static void JsonToSubscriptionEntry(const rapidjson::Value& jsonValue, SubscriptionEntry& entry);
    static void JsonToSubscriptionOffset(const rapidjson::Value& jsonValue, SubscriptionOffset& entry);
    static void SubscriptionOffsetToJson(const SubscriptionOffset& entry, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& jsonValue);
    static void JsonToMeterRecord(const rapidjson::Value& jsonValue, MeterRecord& record);
    static void EncodeBlobValue(const std::string& blobValue, rapidjson::Document::AllocatorType& allocator, rapidjson::Value& value);
    static void DecodeBlobValue(const char* data, int len, std::string& out);
};

} // namespace datahub
} // namespace aliyun
#endif
