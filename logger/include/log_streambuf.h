/*
 * =====================================================================================
 *
 *       Filename:  log_streambuf.h
 *        Created:  06/01/14 19:49:22
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  streambuf subclass for stream style logging
 *
 * =====================================================================================
 */

#ifndef  __LOG_STREAMBUF_H__
#define  __LOG_STREAMBUF_H__

#include <streambuf>

class LogStreambuf : public std::streambuf
{
    public:
        int used();

        int overflow(int c);
    private:
        std::streambuf* setbuf (char* s, std::streamsize n);

    private:
        char tmp_;
};

#endif   /* ----- #ifndef __LOG_STREAMBUF_H__  ----- */
