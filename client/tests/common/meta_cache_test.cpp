#include "config.h"
#include "meta_data.h"
#include "datahub/datahub_client.h"
#include "datahub/datahub_exception.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#define private public
#include "meta_cache.h"
#include "datahub_client_factory.h"

#define TEST_NUM 2
#define REGISTER_NUM 10

using namespace aliyun;
using namespace aliyun::datahub;


// 四个自变量: endpoint, project, topic, subId
// 验证 MetaData由(endpoint,project,topic,subId)唯一指定
class MetaCacheGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
    }

    static void CreateProject(DatahubClient* client, const std::string& project)
    {
        try
        {
            client->CreateProject(project, "test project");
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void CreateTopic(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        try
        {
            client->CreateTopic(project, topic, conf_shard_count, conf_life_cycle, BLOB, "test topic");
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void CreateSubscription(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        try
        {
            const CreateSubscriptionResult& csr = client->CreateSubscription(project, topic, "test subscription");
            std::cout << "Created subId: " << csr.GetSubId() << ", project:" << project << ", topic: " << topic << std::endl;
            subId = csr.GetSubId();
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void DeleteTopic(DatahubClient* client, const std::string& project, const std::string& topic)
    {
        ListSubscriptionResult lsr = client->ListSubscription(project, topic, 1, 10);
        const std::vector<SubscriptionEntry>& ls = lsr.GetSubscriptions();
        for (auto subIt = ls.begin(); subIt != ls.end(); subIt++)
        {
            try
            {
                client->DeleteSubscription(project, topic, subIt->GetSubId());
            }
            catch (const DatahubException& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }
        try
        {
            client->DeleteTopic(project, topic);
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    static void DeleteProject(DatahubClient* client, const std::string& project)
    {
        ListTopicResult ltr = client->ListTopic(project);
        const StringVec& lt = ltr.GetTopicNames();
        for (auto it = lt.begin(); it != lt.end(); it++)
        {
            DeleteTopic(client, project, *it);
        }
        try
        {
            client->DeleteProject(project);
        }
        catch (const DatahubException& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    template <typename T>
    static void CheckVector(std::vector<std::vector<T> >& tarVec)
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

    virtual void TearDown()
    {
        MetaCache::GetInstance().mMetaDataCache.clear();
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap.clear();
    }

    static std::string subId;
    static ShardCoordinator* coordinatorPtr;
};

std::string MetaCacheGTest::subId;
ShardCoordinator* MetaCacheGTest::coordinatorPtr = nullptr;

TEST_F(MetaCacheGTest, MultiEndpointTest)
{
    StringVec endpointVec;
    endpointVec.push_back("https://dh-cn-shenzhen.aliyuncs.com");
    endpointVec.push_back("https://dh-cn-hangzhou.aliyuncs.com");
    endpointVec.push_back("https://dh-cn-shanghai.aliyuncs.com");

    Account account;
    account.id = datahub_conf_accessId;
    account.key = datahub_conf_accessKey;
    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < endpointVec.size(); id++)
    {
        CommonConfiguration conf(account, endpointVec[id]);
        DatahubClient* client = new DatahubClient((Configuration)conf);

        sleep(5);
        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, conf_topic);
        CreateSubscription(client, conf_project, conf_topic);

        std::vector<MetaDataPtr> tmp;
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            MetaDataPtr metaData = MetaCache::GetInstance().GetMetaData(conf_project, conf_topic, subId, coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaData);
        }

        metaDataVec.push_back(tmp);
        DeleteTopic(client, conf_project, conf_topic);
        DeleteProject(client, conf_project);
        delete client;
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), endpointVec.size());
}

TEST_F(MetaCacheGTest, MultiProjectTest)
{
    size_t PROJECT_NUM = TEST_NUM;
    StringVec projectVec;
    for (size_t i = 0; i < PROJECT_NUM; i++)
    {
        projectVec.push_back("project_test_" + std::to_string(i));
    }

    Account account;
    account.id = conf_accessId;
    account.key = conf_accessKey;
    CommonConfiguration conf(account, conf_endpoint);
    DatahubClient* client = new DatahubClient((Configuration)conf);
    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < projectVec.size(); id++)
    {
        CreateProject(client, projectVec[id]);
        CreateTopic(client, projectVec[id], conf_topic);
        CreateSubscription(client, projectVec[id], conf_topic);

        std::vector<MetaDataPtr> tmp;
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            auto metaData = MetaCache::GetInstance().GetMetaData(projectVec[id], conf_topic, subId, coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaData);
        }

        metaDataVec.push_back(tmp);
        DeleteTopic(client, projectVec[id], conf_topic);
        DeleteProject(client, projectVec[id]);
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), projectVec.size());
}

TEST_F(MetaCacheGTest, MultiTopicTest)
{
    size_t TOPIC_NUM = TEST_NUM;
    StringVec topicVec;
    for (size_t i = 0; i < TOPIC_NUM; i++)
    {
        topicVec.push_back("topic_test_" + std::to_string(i));
    }

    Account account;
    account.id = conf_accessId;
    account.key = conf_accessKey;
    CommonConfiguration conf(account, conf_endpoint);
    DatahubClient* client = new DatahubClient((Configuration)conf);
    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < topicVec.size(); id++)
    {
        CreateProject(client, conf_project);
        CreateTopic(client, conf_project, topicVec[id]);
        CreateSubscription(client, conf_project, topicVec[id]);

        std::vector<MetaDataPtr> tmp;
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            auto metaData = MetaCache::GetInstance().GetMetaData(conf_project, topicVec[id], subId, coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaData);
        }

        metaDataVec.push_back(tmp);
        DeleteTopic(client, conf_project, topicVec[id]);
        DeleteProject(client, conf_project);
    }
    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), topicVec.size());
}

TEST_F(MetaCacheGTest, MultiSubscriptionTest)
{
    size_t SUBSCRIPTION_NUM = TEST_NUM;

    Account account;
    account.id = conf_accessId;
    account.key = conf_accessKey;
    CommonConfiguration conf(account, conf_endpoint);
    DatahubClient* client = new DatahubClient((Configuration)conf);

    CreateProject(client, conf_project);
    CreateTopic(client, conf_project, conf_topic);

    StringVec subVec;
    for (size_t i = 0; i < SUBSCRIPTION_NUM; i++)
    {
        CreateSubscription(client, conf_project, conf_topic);
        subVec.push_back(subId);
    }

    std::vector<std::vector<MetaDataPtr> > metaDataVec;
    for (size_t id = 0; id < subVec.size(); id++)
    {
        std::vector<MetaDataPtr> tmp;
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            auto metaData = MetaCache::GetInstance().GetMetaData(conf_project, conf_topic, subVec[id], coordinatorPtr, conf);
            ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), id + 1);
            tmp.push_back(metaData);
        }
        metaDataVec.push_back(tmp);
    }

    DeleteTopic(client, conf_project, conf_topic);
    DeleteProject(client, conf_project);

    CheckVector(metaDataVec);
    ASSERT_EQ(MetaCache::GetInstance().mMetaDataCache.size(), subVec.size());
}
