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

#include "sync_logging.h"

static __thread struct tm tm;
static __thread time_t lasttime = 0;
static __thread char time_fmt[256];


LogFormatter::LogFormatter(const char* basename, int lineno)
{
    header_len_ = FormatHeader(basename, lineno);
    stream_.rdbuf()->pubsetbuf(buf+header_len_, LogFormatter::kMaxLogLength-header_len_);
}

LogFormatter::~LogFormatter()
{
    int len = header_len_ + stream_.streambuf()->used();
    buf[len] = '\n';
    len += 1;
    SyncLoggingInst->Write(buf, len);
}

size_t LogFormatter::FormatHeader(const char* basename, int lineno)
{
    struct timeval tv;
    int ret = gettimeofday(&tv, NULL);
    assert (ret == 0);
    (void)ret;
    if (tv.tv_sec != lasttime)
    {
        lasttime = tv.tv_sec;
        if (NULL != localtime_r(&tv.tv_sec, &tm))
        {
            strftime(time_fmt, sizeof time_fmt, "[%Y-%m-%d %H:%M:%S.%%06u %%s:%%d][DEBUG] ", &tm);
        }
    }
    return snprintf(buf, LogFormatter::kMaxLogLength, time_fmt, tv.tv_usec, basename, lineno);
}
