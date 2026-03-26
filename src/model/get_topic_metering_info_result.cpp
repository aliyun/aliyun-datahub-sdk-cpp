#include "rapidjson/document.h"
#include "datahub/datahub_result.h"
#include "json_tool.h"

namespace aliyun
{
namespace datahub
{

GetTopicMeteringInfoResult::GetTopicMeteringInfoResult()
{
}

GetTopicMeteringInfoResult::~GetTopicMeteringInfoResult()
{
}

void GetTopicMeteringInfoResult::DeserializePayload(const std::string& payload)
{
    rapidjson::Document doc;
    doc.Parse(payload.c_str());

    rapidjson::Value::ConstMemberIterator meterDataItr = doc.FindMember("MeterData");
    if (meterDataItr != doc.MemberEnd() && meterDataItr->value.IsArray())
    {
        const rapidjson::Value& meterDataJson = meterDataItr->value;
        for (rapidjson::SizeType i = 0; i < meterDataJson.Size(); ++i)
        {
            const rapidjson::Value& meterRecord = meterDataJson[i];
            if (meterRecord.IsObject())
            {
                MeterRecord record;
                JsonTool::JsonToMeterRecord(meterRecord, record);
                meterData.push_back(record);
            }
        }
    }
}

const std::vector<MeterRecord>& GetTopicMeteringInfoResult::GetMeterData() const
{
    return meterData;
}

} // namespace datahub
} // namespace aliyun
