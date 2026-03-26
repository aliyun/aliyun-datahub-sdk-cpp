#include <string>
#include <memory>
#include <exception>
#include <stdlib.h>
#include "gtest/gtest.h"
#include "datahub/datahub_client.h"
#include "config.h"
#include <iostream>

using namespace aliyun;
using namespace aliyun::datahub;

class OffsetGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        Configuration conf(account, conf_endpoint);
        client = new DatahubClient(conf);
        try
        {
            client->CreateProject(conf_project, "project for topic gtest");
        }
        catch (std::exception& e)
        {
        }
    }

    virtual void TearDown()
    {
        try
        {
            client->DeleteProject(conf_project);
        }
        catch (std::exception& e)
        {
        }
        delete client;
    }

    static DatahubClient* client;
};

DatahubClient* OffsetGTest::client;

TEST_F(OffsetGTest, OffsetTest)
{
    const std::string& fieldName = "test";
    RecordSchema schema;
    schema.AddField(Field(fieldName, BIGINT));

    std::stringstream ss;
    ss << rand();
    const std::string& topic = "test_topic_" + ss.str();
    int shardCount = 3;
    int lifecycle = 7;
    const std::string comment = "test";
    RecordType type = TUPLE;

    client->CreateTopic(conf_project, topic, shardCount, lifecycle, type, schema, comment);
    client->WaitForShardReady(conf_project, topic);

    const std::string subscriptionComment = "test_subscription";
    CreateSubscriptionResult result = client->CreateSubscription(conf_project, topic, subscriptionComment);

    ListShardResult lsr = client->ListShard(conf_project, topic);
    std::vector<ShardEntry> shards = lsr.GetShards();

    std::vector<std::string> shardIds;
    for (size_t i = 0; i < shards.size(); ++i)
    {
        ShardEntry shardEntry = shards[i];
        shardIds.push_back(shardEntry.GetShardId());
    }

    ListSubscriptionResult subscriptionResult = client->ListSubscription(conf_project, topic, 1, 20, result.GetSubId());
    ASSERT_EQ(1l, subscriptionResult.GetTotalCount());

    std::vector<SubscriptionEntry> subscriptions = subscriptionResult.GetSubscriptions();
    ASSERT_EQ(1u, subscriptions.size());


    // Init
    OpenSubscriptionOffsetSessionResult offsetSessionResult =
            client->InitSubscriptionOffsetSession(conf_project, topic, result.GetSubId(), shardIds);

    ASSERT_EQ(3u, offsetSessionResult.GetOffsets().size());

    std::map<std::string, SubscriptionOffset> offsets = offsetSessionResult.GetOffsets();
    std::map<std::string, SubscriptionOffset>::iterator iter;

    // Get
    GetSubscriptionOffsetResult getSubscriptionOffsetResult =
            client->GetSubscriptionOffset(conf_project, topic, result.GetSubId(), shardIds);

    ASSERT_EQ(3u, offsetSessionResult.GetOffsets().size());

    // Update
    int64_t updateTimestamp = 1l;
    int64_t updateSequence = 1l;
    uint32_t updateBatchIndex = 0u;
    std::map<std::string, SubscriptionOffset> updateSubscriptionOffsets;
    for(iter = offsets.begin(); iter != offsets.end(); ++iter) {
        SubscriptionOffset sows = iter->second;

        SubscriptionOffset offset(
                updateTimestamp,
                updateSequence,
                updateBatchIndex,
                sows.GetVersion(),
                sows.GetSessionId());

        updateSubscriptionOffsets.insert(
                std::pair<std::string, SubscriptionOffset>(iter->first, offset));
    }

    client->UpdateSubscriptionOffset(conf_project, topic, result.GetSubId(), updateSubscriptionOffsets);

    GetSubscriptionOffsetResult updatedSubscriptionOffsetResult =
            client->GetSubscriptionOffset(conf_project, topic, result.GetSubId(), shardIds);
    ASSERT_EQ(3u, updatedSubscriptionOffsetResult.GetOffsets().size());

    std::map<std::string, SubscriptionOffset> updatedOffsets = updatedSubscriptionOffsetResult.GetOffsets();
    std::map<std::string, SubscriptionOffset>::iterator updatedIter;
    for(updatedIter = updatedOffsets.begin(); updatedIter != updatedOffsets.end(); ++updatedIter) {
        SubscriptionOffset sowv = updatedIter->second;

        ASSERT_EQ(updateTimestamp, sowv.GetTimestamp());
        ASSERT_EQ(updateSequence, sowv.GetSequence());
    }

    //Reset
    int64_t resetTimestamp = 0l;
    int64_t resetSequence = 0l;
    uint32_t resetBatchIndex = 0u;

    std::map<std::string, SubscriptionOffset> resetSubscriptionOffsets;
    for(iter = offsets.begin(); iter != offsets.end(); ++iter) {
        SubscriptionOffset offset(
                resetTimestamp,
                resetSequence,
                resetBatchIndex);

        resetSubscriptionOffsets.insert(
                std::pair<std::string, SubscriptionOffset>(iter->first, offset));
    }

    client->ResetSubscriptionOffset(conf_project, topic, result.GetSubId(), resetSubscriptionOffsets);

    GetSubscriptionOffsetResult resetSubscriptionOffsetResult =
            client->GetSubscriptionOffset(conf_project, topic, result.GetSubId(), shardIds);
    ASSERT_EQ(3u, resetSubscriptionOffsetResult.GetOffsets().size());

    std::map<std::string, SubscriptionOffset> resetOffsets = resetSubscriptionOffsetResult.GetOffsets();
    std::map<std::string, SubscriptionOffset>::iterator resetIter;
    for(resetIter = resetOffsets.begin(); resetIter != resetOffsets.end(); ++resetIter) {
        SubscriptionOffset sowv = resetIter->second;

        ASSERT_EQ(resetTimestamp, sowv.GetTimestamp());
        ASSERT_EQ(resetSequence, sowv.GetSequence());
    }


    for (size_t i = 0; i < subscriptions.size(); ++i)
    {
        SubscriptionEntry entry = subscriptions[i];

        client->DeleteSubscription(conf_project, topic, entry.GetSubId());
    }

    client->DeleteTopic(conf_project, topic);
}