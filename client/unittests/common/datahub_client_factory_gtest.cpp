#include "gtest/gtest.h"
#include "gmock/gmock.h"
#define private public
#include "datahub_client_factory.h"
#include "datahub/datahub_config.h"

#define REGISTER_NUM 5

using namespace aliyun;
using namespace aliyun::datahub;
using namespace testing;

class DatahubClientFactoryGTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        account.id = "testAccessId";
        account.key = "testAccessKey";
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

TEST_F(DatahubClientFactoryGTest, GetDatahubClientGTest)
{
    std::vector<std::string> endpointVec;
    for (int i = 0; i < 10; i++)
    {
        endpointVec.push_back("http://testEndpoint_" + std::to_string(i));
    }

    ASSERT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), 0u);

    std::vector<std::vector<DatahubClientPtr> > clientPtrVec;
    for (size_t id = 0; id < endpointVec.size(); id++)
    {
        std::vector<DatahubClientPtr> tmp;
        for (size_t i = 0; i < REGISTER_NUM; i++)
        {
            Configuration conf(account, endpointVec[id]);
            conf.SetEnableProtobuf(true);
            DatahubClientPtr client = DatahubClientFactory::GetInstance().GetDatahubClient(conf);
            ASSERT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), id + 1);
            tmp.push_back(client);
        }
        clientPtrVec.push_back(tmp);
    }
    CheckVector(clientPtrVec);
    ASSERT_EQ(DatahubClientFactory::GetInstance().mDatahubClientPtrMap.size(), endpointVec.size());
}
