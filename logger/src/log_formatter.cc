/*
 * =====================================================================================
 *
 *       Filename:  log_formatter.cc
 *        Created:  06/01/14 20:08:20
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "log_formatter.h"

#include <cstdio>
#include <sys/syscall.h>

#include "logger.hpp"

static __thread struct tm tm;
static __thread time_t lasttime = 0;
static __thread char time_fmt[256];
static __thread pid_t tid = 0;

LogFormatter::LogFormatter(const char* basename, const char* funcname, int lineno, const char* level)
{
    header_len_ = FormatHeader(basename, funcname, lineno, level);
    stream_.rdbuf()->pubsetbuf(buf+header_len_, LogFormatter::kMaxLogLength-header_len_-1);
}

LogFormatter::~LogFormatter()
{
    int len = header_len_ + stream_.streambuf()->used();
    buf[len] = '\n';
    len += 1;
    LoggerInst->Write(buf, len);
}

size_t LogFormatter::FormatHeader(const char* basename, const char* funcname, int lineno, const char* level)
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    assert (ret == 0);
    (void)ret;
    if (tid == 0) 
    {
        tid = syscall(SYS_gettid);
    }
    if (tv.tv_sec != lasttime)
    {
        lasttime = tv.tv_sec;
        if (NULL != localtime_r(&tv.tv_sec, &tm))
        {
            strftime(time_fmt, sizeof time_fmt, "[%Y-%m-%d %H:%M:%S.%%06u %%s:%%d %%s %%5.d][%%s] ", &tm);
        }
    }
    return snprintf(buf, LogFormatter::kMaxLogLength, time_fmt, tv.tv_usec, basename, lineno, funcname, tid, level);
}
