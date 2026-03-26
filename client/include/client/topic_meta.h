#ifndef INCLUDE_DATAHUB_CLIENT_TOPIC_META_H
#define INCLUDE_DATAHUB_CLIENT_TOPIC_META_H

#include "datahub/datahub_record.h"
#include <string>


namespace aliyun
{
namespace datahub
{

class TopicMeta
{
public:
    TopicMeta(const std::string& endpoint, const std::string& projectName, const std::string& topicName,
        const std::string& recordType, const RecordSchema& schema)
        : mEndpoint(endpoint),
          mProjectName(projectName),
          mTopicName(topicName),
          mRecordType(recordType),
          mRecordSchema(schema)
    {}

    std::string GetEndpoint() const { return mEndpoint; };
    std::string GetProjectName() const { return mProjectName; };
    std::string GetTopicName() const { return mTopicName; };
    std::string GetRecordType() const { return mRecordType; };
    const RecordSchema& GetRecordSchema() const { return mRecordSchema; };

private:
    std::string mEndpoint;
    std::string mProjectName;
    std::string mTopicName;
    std::string mRecordType;
    RecordSchema mRecordSchema;
};

typedef std::shared_ptr<TopicMeta> TopicMetaPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_TOPIC_META_H
