#include "gtest/gtest.h"
#include "utils.h"

using namespace aliyun;
using namespace aliyun::datahub;

class UtilsGTest : public testing::Test
{
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(UtilsGTest, TestUrlEncode)
{
    std::string result;

    result = Utils::UrlEncode("url?/&");
    ASSERT_EQ("url%3F%2F%26", result);

    result = Utils::UrlEncode("测试url");
    ASSERT_EQ("%E6%B5%8B%E8%AF%95url", result);

    result = Utils::UrlEncode("");
    ASSERT_EQ("", result);

    result = Utils::UrlEncode("testuri");
    ASSERT_EQ("testuri", result);

    result = Utils::UrlEncode("abcdefghijklmnoqprstuvwxyzABCDEFGHIJKLMNOQPRSTUVWXYZ_-~./");
    ASSERT_EQ("abcdefghijklmnoqprstuvwxyzABCDEFGHIJKLMNOQPRSTUVWXYZ_-~.%2F", result);
}

TEST_F(UtilsGTest, TimeTest)
{
    // 2019/9/26 14:0:0
    int64_t time = 1569477600;

    int64_t startTime = Utils::GetStartTimeOfDay(time);
    ASSERT_EQ(startTime, 1569427200);

    int64_t endTime = Utils::GetEndTimeOfDay(time);
    ASSERT_EQ(endTime, 1569513600);

    std::string day = Utils::GetDayFromTimeStamp(time);
    ASSERT_EQ(day, "20190926");

    time_t time1 = Utils::ConvertToTimestamp(day);
    ASSERT_EQ(time1, 1569427200);

    time_t time2 = Utils::ConvertToTimestamp(day, "8");
    ASSERT_EQ(time2, 1569456000);
}

TEST_F(UtilsGTest, RemoveZeroTest)
{
    std::string aa = "1.0000e123";
    Utils::RemoveZero(aa);
    ASSERT_EQ(aa, "1e123");

    aa = "0.0000e123";
    Utils::RemoveZero(aa);
    ASSERT_EQ(aa, "0e123");

    aa = "0.1000e123";
    Utils::RemoveZero(aa);
    ASSERT_EQ(aa, "0.1e123");

    aa = "0.1000e0123";
    Utils::RemoveZero(aa);
    ASSERT_EQ(aa, "0.1e0123");

    aa = "1.00000001e-07";
    Utils::RemoveZero(aa);
    ASSERT_EQ(aa, "1.00000001e-07");
}

TEST_F(UtilsGTest, FloatToStringTest)
{
    float a = 1000000;
    ASSERT_EQ(Utils::FloatToString(a), "1e+06");

    a = 0.0000001;
    ASSERT_EQ(Utils::FloatToString(a), "1.00000001e-07");
}

TEST_F(UtilsGTest, DoubleToStringTest)
{
    double a = 1000000;
    ASSERT_EQ(Utils::DoubleToString(a), "1e+06");

    a = 0.0000001;
    ASSERT_EQ(Utils::DoubleToString(a), "9.9999999999999995e-08");
}

TEST_F(UtilsGTest, SplitTest)
{
    std::vector<std::string> ret;
    ret = Utils::Split("", ':');
    ASSERT_EQ(ret.size(), 1u);
    ASSERT_EQ(ret[0], "");

    ret = Utils::Split(":", ':');
    ASSERT_EQ(ret.size(), 2u);
    ASSERT_EQ(ret[0], "");
    ASSERT_EQ(ret[1], "");

    ret = Utils::Split(" :", ':');
    ASSERT_EQ(ret.size(), 2u);
    ASSERT_EQ(ret[0], " ");
    ASSERT_EQ(ret[1], "");

    ret = Utils::Split(" : aa cc", ':');
    ASSERT_EQ(ret.size(), 2u);
    ASSERT_EQ(ret[0], " ");
    ASSERT_EQ(ret[1], " aa cc");

    ret = Utils::Split(" : aa cc :", ':');
    ASSERT_EQ(ret.size(), 3u);
    ASSERT_EQ(ret[0], " ");
    ASSERT_EQ(ret[1], " aa cc ");
    ASSERT_EQ(ret[2], "");

    ret = Utils::Split(" : aa cc :", 'c');
    ASSERT_EQ(ret.size(), 3u);
    ASSERT_EQ(ret[0], " : aa ");
    ASSERT_EQ(ret[1], "");
    ASSERT_EQ(ret[2], " :");
}
