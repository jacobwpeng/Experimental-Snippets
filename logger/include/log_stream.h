/*
 * =====================================================================================
 *
 *       Filename:  log_stream.h
 *        Created:  06/01/14 19:52:05
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  ostream subclass for stream style logging
 *
 * =====================================================================================
 */

#ifndef  __LOG_STREAM_H__
#define  __LOG_STREAM_H__

#include <ostream>
#include "log_streambuf.h"

class LogStream : public std::ostream
{
    public:
        LogStream();

        LogStreambuf* streambuf();

    private:
        LogStreambuf streambuf_;
};

#endif   /* ----- #ifndef __LOG_STREAM_H__  ----- */
