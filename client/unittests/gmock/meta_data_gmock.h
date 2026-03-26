#ifndef INCLUDE_DATAHUB_CLIENT_META_DATA_GMOCK_H
#define INCLUDE_DATAHUB_CLIENT_META_DATA_GMOCK_H

#include "gmock/gmock.h"
#include "meta_data.h"

namespace aliyun
{
namespace datahub
{

class GMockMetaData : public MetaData
{
public:
    GMockMetaData(const std::string& projectName, const std::string& topicName, const std::string& subId, const CommonConfiguration& conf)
        : MetaData(projectName, topicName, subId, conf) {}
    virtual ~GMockMetaData() {}

    MOCK_METHOD0(Init, void ());
    MOCK_METHOD0(UpdateShardMetaMap, void ());
};

typedef std::shared_ptr<GMockMetaData> GMockMetaDataPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_META_DATA_GMOCK_H