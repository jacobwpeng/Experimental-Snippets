/*
 * =====================================================================================
 *
 *       Filename:  log_cfile.cc
 *        Created:  06/06/14 17:06:31
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "log_cfile.h"
#include <cstdio>                               /* perror */
#include <fcntl.h>                              /* O_WRONLY and ... */

LogCFile::LogCFile(const std::string& prefix, const std::string& suffix, size_t rotate_size, bool thread_safe)
    :fp_(NULL), name_prefix_(prefix), name_suffix_(suffix), rotate_size_(rotate_size), bytes_written_(0),
    mutex_(thread_safe ? new boost::mutex : NULL)
{
    RotateFile();
}

LogCFile::~LogCFile()
{
    if (fp_) fclose(fp_);
}

void LogCFile::Append(const char* buf, size_t len)
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

void LogCFile::NonLockAppend(const char* buf, size_t len)
{
    if (bytes_written_ >= rotate_size_)
    {
        RotateFile();
    }
    assert (fp_ != NULL);
    int bytes = fwrite(buf, 1, len, fp_);
    if (bytes < 0)
    {
        perror("write");
    }
    else
    {
        bytes_written_ += bytes;
    }
    NonLockFlush();
}

void LogCFile::Flush()
{
    if (mutex_)
    {
        boost::mutex::scoped_lock lock(*mutex_);
        NonLockFlush();
    }
    else
    {
        NonLockFlush();
    }
}

void LogCFile::NonLockFlush()
{
    assert (fp_);
    //fflush(fp_);
}

void LogCFile::RotateFile()
{
    if (fp_)
    {
        fclose(fp_);
    }
    std::string filename = MakeLogFilename(name_prefix_, name_suffix_);
    fp_ = fopen(filename.c_str(), "a");
    if (fp_ == NULL)
    {
        perror("fopen");
        assert (false);
    }
    else
    {
        setbuf(fp_, NULL);
    }
    bytes_written_ = 0;
}
