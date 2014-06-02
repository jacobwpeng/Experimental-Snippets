/*
 * =====================================================================================
 *
 *       Filename:  log_formatter.h
 *        Created:  06/01/14 19:55:00
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  class for format logging messages
 *
 * =====================================================================================
 */

#ifndef  __LOG_FORMATTER_H__
#define  __LOG_FORMATTER_H__

#include "log_stream.h"

struct BasenameRetriever
{
    template<int len>
    BasenameRetriever(const char (&arr)[len])
    :basename(NULL)
    {
        int pos = len - 1;                      /* skil '\0' */
        while (pos >= 0)
        {
            if (arr[pos] == '/')
            {
                basename = arr + pos + 1;
                break;
            }
            else
            {
                --pos;
            }
        }
        if (basename == NULL) basename = arr;
    }
    const char* basename;
};

class LogFormatter
{
    public:
        LogFormatter(const char* basename, int lineno, const char* level);
        ~LogFormatter();
        std::ostream& stream() { return stream_; }

    private:
        size_t FormatHeader(const char* basename, int lineno, const char* level);

    private:
        static const size_t kMaxLogLength = 1 << 12;
        LogStream stream_;
        size_t header_len_;
        char buf[kMaxLogLength];
};

#endif   /* ----- #ifndef __LOG_FORMATTER_H__  ----- */
