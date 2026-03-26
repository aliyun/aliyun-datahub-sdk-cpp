#include <string>
#include "gtest/gtest.h"
#include "datahub/datahub_client.h"
#include "config.h"

using namespace aliyun;
using namespace aliyun::datahub;

class ProjectGTest: public testing::Test
{
protected:
    virtual void SetUp()
    {
        Account account;
        account.id = conf_accessId;
        account.key = conf_accessKey;
        Configuration conf(account, conf_endpoint);
        client = new DatahubClient(conf);
    }

    virtual void TearDown()
    {
        delete client;
    }

    static DatahubClient* client;
};

DatahubClient* ProjectGTest::client;

TEST_F(ProjectGTest, ProjectTest)
{
    std::string project = "project_test";
    std::string comment = "comment_test";
    client->CreateProject(project, comment);

    /* Get Project */
    const GetProjectResult& projectResult = client->GetProject(project);
    std::cout<<projectResult.GetProject()<<std::endl;
    std::cout<<projectResult.GetComment()<<std::endl;
    std::cout<<projectResult.GetCreator()<<std::endl;
    std::cout<<projectResult.GetCreateTime()<<std::endl;
    std::cout<<projectResult.GetLastModifyTime()<<std::endl;

    std::cout<<projectResult.GetRequestId()<<std::endl;
    ASSERT_EQ(projectResult.GetRequestId().size() > 0, true);

    ASSERT_EQ(projectResult.GetProject(), project);
    ASSERT_EQ(projectResult.GetComment(), comment);

    sleep(1);

    std::string updateComment = "comment_test_1";
    client->UpdateProject(project, updateComment);

    sleep(1);

    const GetProjectResult& projectResult4Update = client->GetProject(project);
    std::cout<<projectResult4Update.GetComment()<<std::endl;
    std::cout<<projectResult4Update.GetLastModifyTime()<<std::endl;

    ASSERT_EQ(projectResult4Update.GetComment(), updateComment);

    const ListProjectResult& listProjectResult = client->ListProject();
    ASSERT_EQ(listProjectResult.GetProjectNames().size() >= 1, true);

    client->DeleteProject(project);
}
