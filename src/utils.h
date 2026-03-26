#ifndef DATAHUB_SDK_UTILS_H
#define DATAHUB_SDK_UTILS_H

#include <string>
#include <vector>

namespace aliyun
{
namespace datahub
{

class Utils
{
public:
    static std::string GetDate();

    static std::vector<std::string> Split(const std::string& str, char delimiter);

    static std::string GetLocalIP();

    static std::string GenUserAgent(const std::string& client, const std::string& version);

    static std::string& LeftTrim(std::string &s);

    static std::string& RightTrim(std::string &s);

    static std::string& Trim(std::string &s);

    static std::string ToLowerCaseString(const std::string& orig);
    static std::string ToUpperCaseString(const std::string& orig);

    static std::string BoolToString(const bool b);
    static bool StringToBool(const std::string& s);

    static unsigned char ToHex(unsigned char x);
    static std::string UrlEncode(const std::string& str);

    static std::string DoubleToString(const double value);
    static std::string FloatToString(const float value);

    static time_t GetStartTimeOfDay(time_t t);
    static time_t GetEndTimeOfDay(time_t t);
    static std::string GetTimeStamp(time_t t, const std::string& format = "%Y%m%d%H%M%S", bool gmt = false);
    static std::string GetDayFromTimeStamp(time_t t, bool gmt = false);
    static time_t ConvertToTimestamp(const std::string& day);
    static time_t ConvertToTimestamp(const std::string& day, const std::string& hour);

    static void RemoveZero(std::string& str);
};

} // namespace datahub
} // namespace aliyun
#endif
