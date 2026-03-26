#include "logger.h"
#include <algorithm>
#include "log4cpp/Category.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/PatternLayout.hh"
#include "datahub/datahub_exception.h"

namespace aliyun
{
namespace datahub
{

Logger::Logger() : mLogLevel(INFO), mLogFilePath("") {}

log4cpp::Category* Logger::GenLogger(const std::string& name)
{
    log4cpp::Priority::PriorityLevel priorityLevel = log4cpp::Priority::INFO;
    switch (mLogLevel)
    {
    case DEBUG:
        priorityLevel = log4cpp::Priority::DEBUG;
        break;
    case INFO:
        priorityLevel = log4cpp::Priority::INFO;
        break;
    case WARN:
        priorityLevel = log4cpp::Priority::WARN;
        break;
    case ERROR:
        priorityLevel = log4cpp::Priority::ERROR;
        break;
    default:
        break;
    }

    if (mLoggerPtrCache.count(name) == 0)
    {
        std::lock_guard<std::mutex> lock(mGenMutex);
        if (mLoggerPtrCache.count(name) == 0)
        {
            log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
            layout->setConversionPattern("%d{%Y %m %d-%H:%M:%S,%l} [%p] [%t] %c: %m.%n");

            log4cpp::Category* logger = &(log4cpp::Category::getInstance(name));
            logger->setAdditivity(false);

            if (mLogFilePath.empty())
            {
                log4cpp::Appender* appenderOstream = new log4cpp::OstreamAppender("AppenderOstream", &std::cout);
                appenderOstream->setLayout(layout);
                logger->setAppender(appenderOstream);
            }
            else
            {
                log4cpp::Appender* appenderFile = new log4cpp::FileAppender("AppenderFile", mLogFilePath);
                appenderFile->setLayout(layout);
                logger->setAppender(appenderFile);
            }

            logger->setPriority(priorityLevel);
            mLoggerPtrCache[name] = logger;
        }
    }
    return mLoggerPtrCache.at(name);
}

void Logger::SetLogLevel(const std::string& logLevel)
{
    if (logLevel.empty())
    {
        mLogLevel = INFO;
        return ;
    }

    std::string level(logLevel.size(), 0);
    std::transform(logLevel.begin(), logLevel.end(), level.begin(), ::tolower);
    if (level == "debug")
    {
        mLogLevel = DEBUG;
    }
    else if (level == "info")
    {
        mLogLevel = INFO;
    }
    else if (level == "warn" || level == "warning")
    {
        mLogLevel = WARN;
    }
    else if (level == "error")
    {
        mLogLevel = ERROR;
    }
    else
    {
        mLogLevel = INFO;
    }
}

Logger& Logger::GetInstance()
{
    static Logger sLogger;
    return sLogger;
}

} // namespace datahub
} // namespace aliyun
