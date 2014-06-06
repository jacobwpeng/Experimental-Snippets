/*
 * =====================================================================================
 *
 *       Filename:  log_file.cc
 *        Created:  06/01/14 18:01:54
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  file class for logging
 *
 * =====================================================================================
 */

#include "log_file.h"
#include <cstdio>                              /* snprintf */
#include <cstring>                              /* memset, memcpy */
#include <sys/time.h>                           /* gettimeofday */

std::string MakeLogFilename(const std::string& prefix, const std::string& suffix)
{
    struct timeval tv;
    struct tm tm;
    char buf[64];
    char fmt[64];

    memset(buf, 0, sizeof(buf));
    memset(fmt, 0, sizeof(fmt));

    std::string res(prefix);
    int ret = gettimeofday(&tv, NULL);
    if (ret < 0)
    {
        perror("gettimeofday");
    }
    else if (NULL != localtime_r(&tv.tv_sec, &tm))
    {
        strftime(fmt, sizeof fmt, ".%Y%m%d%H%M%S%%06u", &tm);
        snprintf(buf, sizeof buf, fmt, tv.tv_usec);
        res += buf;
        if (not suffix.empty())
        {
            res += "." + suffix;
        }
    }
    else
    {
        perror("localtime_r");
    }
    return res;
}
