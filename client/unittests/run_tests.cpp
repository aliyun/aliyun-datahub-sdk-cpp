#include "gtest/gtest.h"
#include "logger.h"

using namespace aliyun::datahub;

int main(int argc, char** argv)
{
    Logger::GetInstance().SetLogLevel("ERROR");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
