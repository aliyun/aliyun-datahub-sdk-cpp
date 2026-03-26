#ifndef INCLUDE_DATAHUB_CLIENT_LOGGER_H
#define INCLUDE_DATAHUB_CLIENT_LOGGER_H

#include <mutex>
#include <string>
#include <unordered_map>


namespace log4cpp
{
    class Category;
}

#define LOG_DEBUG(_LOGGER, ...)     do {                            \
        if (_LOGGER->getPriority() >= log4cpp::Priority::DEBUG)     \
        {                                                           \
            _LOGGER->debug(__VA_ARGS__);                            \
        }                                                           \
    } while (0)

#define LOG_INFO(_LOGGER, ...) _LOGGER->info(__VA_ARGS__)
#define LOG_WARN(_LOGGER, ...) _LOGGER->warn(__VA_ARGS__)

namespace aliyun
{
namespace datahub
{

enum LogPriorityLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger
{
public:
    log4cpp::Category* GenLogger(const std::string& name);
    static Logger& GetInstance();

    void SetLogLevel(const std::string& logLevel);
    void SetLogFilePath(const std::string& filePath) { mLogFilePath = filePath; }

private:
    Logger();

private:
    LogPriorityLevel mLogLevel;
    std::string mLogFilePath;
    std::unordered_map<std::string, log4cpp::Category*> mLoggerPtrCache;
    std::mutex mGenMutex;
};

} // namespace datahub
} // namespace aliyun

#endif // INCLUDE_DATAHUB_CLIENT_LOGGER_H