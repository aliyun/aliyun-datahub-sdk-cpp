#include "config.h"
#include "datahub/datahub_client.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#define private public
#include "meta_cache.h"
#include "datahub_client_factory.h"

#define REGISTER_NUM 10

using namespace aliyun;
using namespace aliyun::datahub;

class DatahubClientFactoryGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        account.id = conf_accessId;
        account.key = conf_accessKey;
    }

    virtual void TearDown()
    {
        MetaCache::GetInstance().mMetaDataCache.clear();
        DatahubClientFactory::GetInstance().mDatahubClientPtrMap.clear();
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

    static Account account;
};

Account DatahubClientFactoryGTest::account;

TEST_F(DatahubClientFactoryGTest, DatahubClientFactoryTest)
{
    StringVec endpointVec;
    endpointVec.push_back("https://dh-cn-hangzhou.aliyuncs.com");
    endpointVec.push_back("https://dh-cn-shanghai.aliyuncs.com");
    endpointVec.push_back("https://dh-cn-shenzhen.aliyuncs.com");

    std::vector<std::vector<DatahubClientPtr> > clientPtrVec;
    for (size_t id = 0; id < endpointVec.size(); id++)
    {
        std::vector<DatahubClientPtr> tmp;
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            Configuration conf(account, endpointVec[id]);
            DatahubClientPtr client = DatahubClientFactory::GetInstance().GetDatahubClient(conf);
            ASSERT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), id + 1);
            tmp.push_back(client);
        }
        clientPtrVec.push_back(tmp);
    }
    CheckVector(clientPtrVec);
    ASSERT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), endpointVec.size());
}
