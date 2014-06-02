/*
 * =====================================================================================
 *
 *       Filename:  log_streambuf.cc
 *        Created:  06/01/14 19:50:27
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  streambuf subclass for stream style logging
 *
 * =====================================================================================
 */

#include "log_streambuf.h"

int LogStreambuf::used()
{
    return pptr() - pbase();
}

int LogStreambuf::overflow(int c)
{
    return epptr() == pptr() ? tmp_ : c;
}

std::streambuf* LogStreambuf::setbuf (char* s, std::streamsize n)
{
    setp(s, s+n);
    return this;
}
