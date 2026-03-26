#ifndef INCLUDE_DATAHUB_CLIENT_CONSUMER_COORDINATOR_GMOCK_H
#define INCLUDE_DATAHUB_CLIENT_CONSUMER_COORDINATOR_GMOCK_H

#include "consumer_coordinator.h"

namespace aliyun
{
namespace datahub
{

class GMockOffsetCoordinator : public OffsetCoordinator
{
public:
    GMockOffsetCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf)
        : OffsetCoordinator(projectName, topicName, subId, consumerConf)
    {}
    virtual ~GMockOffsetCoordinator() {}

    MOCK_METHOD1(SetAssignShards, void (const StringVec&));
};

typedef std::shared_ptr<GMockOffsetCoordinator> GMockOffsetCoordinatorPtr;

class GMockConsumerCoordinator : public ConsumerCoordinator
{
public:
    GMockConsumerCoordinator(const std::string& projectName, const std::string& topicName, const std::string& subId, const ConsumerConfiguration& consumerConf)
        : ConsumerCoordinator(projectName, topicName, subId, consumerConf)
    {}
    virtual ~GMockConsumerCoordinator() {}

    MOCK_METHOD2(OnShardChange, void (const StringVec&, const StringVec&));

};

typedef std::shared_ptr<GMockConsumerCoordinator> GMockConsumerCoordinatorPtr;

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_CONSUMER_COORDINATOR_GMOCK_H