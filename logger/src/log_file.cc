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

#include <cstdio>

LogFile::LogFile(const std::string& logname, size_t rotate_size, bool thread_safe)
    :fp_(NULL), logname_(logname), rotate_size_(rotate_size), bytes_written_(0),
    mutex_(thread_safe ? new boost::mutex() : NULL)
{
    std::string filename = MakeFilename();
    fp_ = fopen(filename.c_str(), "as");
    assert (fp_ != NULL);
    setbuf(fp_, NULL);                          /* disable std buffer */
}

LogFile::~LogFile()
{
    if (fp_)
    {
        fclose(fp_);
    }
}

void LogFile::Append(const char* buf, size_t len)
{
    if (mutex_)
    {
        boost::mutex::scoped_lock lock(*mutex_);
        NonLockAppend(buf, len);
    }
    else
    {
        NonLockAppend(buf, len);
    }
}

void LogFile::NonLockAppend(const char* buf, size_t len)
{
    size_t bytes = fwrite(buf, 1, len, fp_);
    bytes_written_ += bytes;
    Flush();

    if (bytes_written_ > rotate_size_)
    {
        RotateFile();
        bytes_written_ = 0u;
    }
}

void LogFile::Flush()
{
    fflush(fp_);
}

std::string LogFile::MakeFilename()
{
    struct timeval tv;
    struct tm tm;
    char buf[64];
    char fmt[64];

    memset(buf, 0, sizeof(buf));
    memset(fmt, 0, sizeof(fmt));

    int ret = gettimeofday(&tv, NULL);
    if (ret < 0)
    {
        perror("gettimeofday");
    }
    else if (NULL != localtime_r(&tv.tv_sec, &tm))
    {
        strftime(fmt, sizeof fmt, ".%Y%m%d%H%M%S%%06u", &tm);
        snprintf(buf, sizeof buf, fmt, tv.tv_usec);
        return logname_ + buf;
    }
    else
    {
        perror("localtime_r");
    }
    return logname_;
}

void LogFile::RotateFile()
{
    assert (fp_ != NULL);
    fclose(fp_);

    std::string filename = MakeFilename();
    fp_ = fopen(filename.c_str(), "as");
    assert (fp_ != NULL);
}
