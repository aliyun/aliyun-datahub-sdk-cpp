#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <time.h>
#include <string.h>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#include <locale>
#include <cassert>

#include "utils.h"

namespace aliyun
{
namespace datahub
{

const char* XSTREAM_RECYCLER_DEFAULT_DATE_FORMAT = "%Y%m%d";

std::string Utils::GetDate()
{
    struct tm result;
    time_t timer = time(NULL);
    gmtime_r(&timer, &result);

    /* RFC822 format date */
    char buffer[50] = {0};
    strftime(buffer, 50, "%a, %d %b %Y %H:%M:%S GMT", &result);
    return std::string(buffer);
}

std::vector<std::string> Utils::Split(const std::string& str, char delimiter)
{
    size_t start = 0;
    std::vector<std::string> ret;
    do
    {
        size_t pos = str.find(delimiter, start);
        if (pos != std::string::npos)
        {
            ret.emplace_back(std::string(str.c_str() + start, pos - start));
            start = pos + 1;
        }
        else
        {
            ret.emplace_back(std::string(str.c_str() + start, str.size() - start));
            break;
        }
    } while (true);
    return ret;
}

std::string Utils::GetLocalIP(){

    int sockfd;
    struct ifconf ifconf;
    struct ifreq *ifreq;
    char buf[512];

    ifconf.ifc_len =512;
    ifconf.ifc_buf = buf;
    if ((sockfd =socket(AF_INET,SOCK_DGRAM,0))<0)
    {
       return "0.0.0.0";
    }

    ioctl(sockfd, SIOCGIFCONF, &ifconf);
    close(sockfd);

    ifreq = (struct ifreq*)ifconf.ifc_buf;
    int total = ifconf.ifc_len / sizeof(struct ifreq);

    for (int i = 0; i < total ; ++i)
    {
        if((ifreq + i)->ifr_flags == AF_INET && strncmp((ifreq + i)->ifr_name, "lo", 2) != 0)
        {
            return inet_ntoa(((struct sockaddr_in*) &((ifreq + i)->ifr_addr))->sin_addr);
        }
    }
    return "0.0.0.0";
}

std::string Utils::GenUserAgent(const std::string& client, const std::string& version)
{
    std::string result;
    std::string localIP = GetLocalIP();

    result.append(client).append("/").append(version).append("/").append(localIP);
    return result;
}

std::string& Utils::LeftTrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

std::string& Utils::RightTrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

std::string& Utils::Trim(std::string &s)
{
    return LeftTrim(RightTrim(s));
}

std::string Utils::ToLowerCaseString(const std::string& orig)
{
    std::string result;

    std::locale loc;
    for (unsigned int i = 0; i < orig.length(); ++i)
    {
        result += std::tolower(orig.at(i), loc);
    }

    return result;
}

std::string Utils::ToUpperCaseString(const std::string& orig)
{
    std::string result;

    std::locale loc;
    for (unsigned int i = 0; i < orig.length(); ++i)
    {
        result += std::toupper(orig.at(i), loc);
    }

    return result;
}

std::string Utils::BoolToString(const bool b)
{
    return b ? "true" : "false";
}

bool Utils::StringToBool(const std::string& s)
{
    if (s == "true" || s == "1")
    {
        return true;
    }

    return false;
}

unsigned char Utils::ToHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}

std::string Utils::UrlEncode(const std::string& str)
{
    std::string strTemp;
    size_t length = str.length();
    for (size_t i = 0; i < length; ++i)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
        {
            strTemp += str[i];
        }
        else if (str[i] == ' ')
        {
            strTemp += "+";
        }
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

std::string Utils::DoubleToString(const double value)
{
    // +-1._1234567890123456_e_+-308_\0
    // at most 1 + 2 + 16 + 1 + 4 + 1 = 25
    // 0._e+_00_\0
    // at least 2 + 2 + 2 + 1 = 7
    std::string ret(32, 0);

    char* buffer = &ret[0];
    size_t cnt = snprintf(buffer, 32, "%.16e", value);
    ret.resize(cnt);

    RemoveZero(ret);
    return ret;
}

std::string Utils::FloatToString(const float value)
{
    // +-1._12345678_e_+-38_\0
    // at most 1 + 2 + 8 + 2 + 3 + 1 = 17
    // 0._e+_00_\0
    // at least 2 + 2 + 2 + 1 = 7

    std::string ret(32, 0);

    char* buffer = &ret[0];
    size_t cnt = snprintf(buffer, 32, "%.8e", value);
    ret.resize(cnt);

    RemoveZero(ret);
    return ret;
}

void Utils::RemoveZero(std::string& str)
{
    char* buffer = &str[0];
    size_t pos = str.size();

    while (pos > 2 && buffer[--pos] != 'e');
    size_t remain = str.size() - pos;

    while (pos > 1 && buffer[--pos] == '0');
    if (buffer[pos] == '.')
    {
        pos--;
    }

    memmove(buffer + pos + 1, buffer + str.size() - remain, remain);
    str.resize(pos + 1 + remain);
}

time_t Utils::GetStartTimeOfDay(time_t t)
{
    struct tm tm;
    localtime_r(&t, &tm);
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    tm.tm_isdst = -1; // -1 used when dst info is unknown

    return mktime(&tm);
}

time_t Utils::GetEndTimeOfDay(time_t t)
{
    struct tm tm;
    localtime_r(&t, &tm);
    tm.tm_hour = 23;
    tm.tm_min = 59;
    tm.tm_sec = 60;
    tm.tm_isdst = -1; // -1 used when dst info is unknown

    return mktime(&tm);
}

std::string Utils::GetTimeStamp(time_t t, const std::string& format, bool gmt)
{
    char buffer[32] = {0};
    time_t timer = t;
    struct tm result;
    struct tm* ret = gmt ? gmtime_r(&timer, &result) : localtime_r(&timer, &result);
    strftime(buffer, sizeof(buffer), format.c_str(), ret);
    return std::string(buffer);
}

std::string Utils::GetDayFromTimeStamp(time_t t, bool gmt)
{
    return GetTimeStamp(t, "%Y%m%d", gmt);
}

time_t Utils::ConvertToTimestamp(const std::string& day)
{
    return ConvertToTimestamp(day, "");
}

time_t Utils::ConvertToTimestamp(const std::string& day, const std::string& hour)
{
    char buf[256] = {0};
    const char* format = XSTREAM_RECYCLER_DEFAULT_DATE_FORMAT;

    struct tm tm;
    strptime(day.c_str(), format, &tm);
    strftime(buf, 256, format, &tm);
    if (strcmp(buf, day.c_str()) != 0)
    {
        return 0;
    }

    tm.tm_hour = hour.empty() ? 0 : std::stoi(hour);
    tm.tm_min = tm.tm_sec = 0;
    tm.tm_isdst = -1; // -1 used when dst info is unknown

    return mktime(&tm);
}

} // namespace datahub
} // namespace aliyun

