/*
 * =====================================================================================
 *
 *       Filename:  log_stream.cc
 *        Created:  06/01/14 19:53:15
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  streambuf subclass for stream style logging
 *
 * =====================================================================================
 */

#include "log_stream.h"

LogStream::LogStream()
{
    rdbuf(&streambuf_);
}

LogStreambuf* LogStream::streambuf()
{
    return &streambuf_;
}
