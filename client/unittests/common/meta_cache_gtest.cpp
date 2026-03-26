#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "gmock/result_gmock.h"
#include "client/common_config.h"
#define private public
#include "meta_cache.h"
#include "datahub_client_factory.h"
#include "gmock/datahub_client_gmock.h"

#define REGISTER_NUM 5

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class MetaCacheGTest : public Test
{
protected:
    virtual void SetUp()
    {
        account.id = "testAccessId";
        account.key = "testAccesskey";
    }

    void ClearCache()
    {
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap.clear();
        MetaCache::GetInstance().mMetaDataCache.clear();
    }

    template <typename T>
    static void CheckVector(std::vector<std::vector<T> >& tarVec, bool allEQ = false)
    {
        for (size_t i = 0; i < tarVec.size(); i++)
        {
            for (size_t j = 0; j < tarVec[i].size(); j++)
            {
                if (i > 0)
                {
                    ASSERT_NE(tarVec[i][j], tarVec[i-1][j]);
                }
                if (j > 0)
                {
                    ASSERT_EQ(tarVec[i][j], tarVec[i][j-1]);
                }
            }
        }
    }

    static Account account;
    static ShardCoordinator* coordinatorPtr;
};

Account MetaCacheGTest::account;
ShardCoordinator* MetaCacheGTest::coordinatorPtr = nullptr;

TEST_F(MetaCacheGTest, MultiSuccessEndpointTest)
{
    ClearCache();
    StringVec endpointVec;
    for (int i = 0; i < 10; i++)
    {
        endpointVec.push_back("http://successEndpoint_" + std::to_string(i));
    }

    std::string projectName = "successProject";
    std::string topicName = "successTopic";
    std::string subId = "successSubId";

    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), 0u);
    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 0u);

    std::vector<GMockDatahubClientPtr> GMockClientVec;
    for (size_t id = 0; id < endpointVec.size(); id++)
    {
        Configuration conf(account, endpointVec[id]);
        std::string key;
        key.append(endpointVec[id]).append(":").append(account.id).append(":").append(account.key);
        GMockClientVec.push_back(std::make_shared<GMockDatahubClient>(conf));
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = GMockClientVec[id];
    }

    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), endpointVec.size());

    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < endpointVec.size(); id++)
    {
        CommonConfiguration conf(account, endpointVec[id]);
        std::vector<MetaDataPtr> tmp;
        EXPECT_CALL(*GMockClientVec[id].get(), GetTopic(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetGetTopicResult()));
        EXPECT_CALL(*GMockClientVec[id].get(), ListShard(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            MetaDataPtr metaDataPtr = MetaCache::GetInstance().GetMetaData(projectName, topicName, subId, coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaDataPtr);
        }
        metaDataVec.push_back(tmp);
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), endpointVec.size());
}

TEST_F(MetaCacheGTest, MultiSuccessProjectTest)
{
    ClearCache();
    StringVec projectVec;
    for (int i = 0; i < 10; i++)
    {
        projectVec.push_back("successProject_" + std::to_string(i));
    }

    std::string endpoint = "http://successEndpoint";
    std::string topicName = "successTopic";
    std::string subId = "successSubId";

    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), 0u);
    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 0u);

    Configuration conf(account, endpoint);
    std::string key;
    key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
    GMockDatahubClientPtr datahubClientPtr = std::make_shared<GMockDatahubClient>(conf);
    DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;

    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 1u);

    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < projectVec.size(); id++)
    {
        CommonConfiguration conf(account, endpoint);
        std::vector<MetaDataPtr> tmp;
        EXPECT_CALL(*datahubClientPtr.get(), GetTopic(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetGetTopicResult()));
        EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            MetaDataPtr metaDataPtr = MetaCache::GetInstance().GetMetaData(projectVec[id], topicName, subId, coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaDataPtr);
        }
        metaDataVec.push_back(tmp);
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), projectVec.size());
}

TEST_F(MetaCacheGTest, MultiSuccessTopicTest)
{
    ClearCache();
    StringVec topicVec;
    for (int i = 0; i < 10; i++)
    {
        topicVec.push_back("successTopic_" + std::to_string(i));
    }

    std::string endpoint = "http://successEndpoint";
    std::string projectName = "successProject";
    std::string subId = "successSubId";

    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), 0u);
    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 0u);

    Configuration conf(account, endpoint);
    std::string key;
    key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
    GMockDatahubClientPtr datahubClientPtr = std::make_shared<GMockDatahubClient>(conf);
    DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;

    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 1u);

    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < topicVec.size(); id++)
    {
        CommonConfiguration conf(account, endpoint);
        std::vector<MetaDataPtr> tmp;
        EXPECT_CALL(*datahubClientPtr.get(), GetTopic(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetGetTopicResult()));
        EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            MetaDataPtr metaDataPtr = MetaCache::GetInstance().GetMetaData(projectName, topicVec[id], subId, coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaDataPtr);
        }
        metaDataVec.push_back(tmp);
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), topicVec.size());
}

TEST_F(MetaCacheGTest, MultiSuccessSubIdTest)
{
    ClearCache();
    StringVec subIdVec;
    for (int i = 0; i < 10; i++)
    {
        subIdVec.push_back("successSubId_" + std::to_string(i));
    }

    std::string endpoint = "http://successEndpoint";
    std::string projectName = "successProject";
    std::string topicName = "successTopic";

    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), 0u);
    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 0u);

    Configuration conf(account, endpoint);
    std::string key;
    key.append(endpoint).append(":").append(account.id).append(":").append(account.key);
    GMockDatahubClientPtr datahubClientPtr = std::make_shared<GMockDatahubClient>(conf);
    DatahubClientFactory::GetInstance().mDatahubClientPtrMap[key] = datahubClientPtr;

    EXPECT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 1u);

    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < subIdVec.size(); id++)
    {
        CommonConfiguration conf(account, endpoint);
        std::vector<MetaDataPtr> tmp;
        EXPECT_CALL(*datahubClientPtr.get(), GetTopic(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetGetTopicResult()));
        EXPECT_CALL(*datahubClientPtr.get(), ListShard(::testing::_, ::testing::_)).
                Times(1).
                WillOnce(::testing::Return(GMockResult::GetInstance().GetListShardResult()));
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            MetaDataPtr metaDataPtr = MetaCache::GetInstance().GetMetaData(projectName, topicName, subIdVec[id], coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaDataPtr);
        }
        metaDataVec.push_back(tmp);
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), subIdVec.size());
}