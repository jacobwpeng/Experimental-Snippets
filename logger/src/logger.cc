/*
 * =====================================================================================
 *
 *       Filename:  logger.cc
 *        Created:  05/29/14 22:16:52
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "logger.h"

#include <time.h>
#include <sys/time.h>
#include <cstdio>
#include <cassert>

namespace detail
{
    const char* basename(const char* filename)
    {
        unsigned pos = 0;
        unsigned where = 0;
        char c;
        while ( (c = filename[pos]) != '\0' )
        {
            if (c == '/') where = pos;
            ++pos;
        }
        return filename + where + 1;
    }

    LogStream::LogStream(Logger::LogLevel level, const char* filename, int line)
        :level_(level), filename_(filename), line_(line)
    {
        assert (level_ < Logger::kLogMax);
        len = 0;
        FormatHeader();
    }

    LogStream::~LogStream()
    {
        buf[len] = '\n';
        len += 1;
        Logger::Instance()->Write(buf, len);
    }

    void LogStream::FormatHeader()
    {
        struct timeval tv;
        struct tm tm;

        int ret = gettimeofday(&tv, NULL);
        assert (ret == 0);

        if (NULL != localtime_r(&tv.tv_sec, &tm))
        {
            strftime(fmt, sizeof fmt, "[%Y-%m-%d %H:%M:%S.%%06u %%s:%%d][%%s] ", &tm);
            len += snprintf(buf + len, sizeof(buf) - len, fmt, tv.tv_usec, basename(filename_), line_, Logger::LogLevelName[level_]);
        }
    }
}

const char * Logger::LogLevelName[Logger::kLogMax];

Logger::Logger()
{
}

Logger::~Logger()
{
}

Logger* Logger::Instance()
{
    static Logger logger;
    return &logger;
}

void Logger::Init()
{
    (void)Logger::Instance();
    LogLevelName[kLogError] = "ERROR";
    LogLevelName[kLogWarning] = "WARNING";
    LogLevelName[kLogInfo] = "INFO";
    LogLevelName[kLogDebug] = "DEBUG";
}

void Logger::Write(const char* buf, unsigned len)
{
    printf("%.*s", len, buf);
}
