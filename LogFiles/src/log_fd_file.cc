/*
 * =====================================================================================
 *
 *       Filename:  log_fd_file.cc
 *        Created:  06/06/14 15:58:36
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "log_fd_file.h"
#include <cstdio>                               /* perror */
#include <fcntl.h>                              /* O_WRONLY and ... */

LogFdFile::LogFdFile(const std::string& prefix, const std::string& suffix, size_t rotate_size, bool thread_safe)
    :fd_(-1), name_prefix_(prefix), name_suffix_(suffix), rotate_size_(rotate_size), bytes_written_(0),
    mutex_(thread_safe ? new boost::mutex : NULL)
{
    RotateFile();
}

LogFdFile::~LogFdFile()
{
    if (fd_ >= 0) close(fd_);
}

void LogFdFile::Append(const char* buf, size_t len)
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

void LogFdFile::NonLockAppend(const char* buf, size_t len)
{
    if (bytes_written_ >= rotate_size_)
    {
        RotateFile();
    }
    ssize_t bytes = write(fd_, buf, len);
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

void LogFdFile::Flush()
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

void LogFdFile::NonLockFlush()
{
    assert (fd_ >= 0);
    //fdatasync(fd_);
}

void LogFdFile::RotateFile()
{
    if (fd_ >= 0)
    {
        close(fd_);
    }
    std::string filename = MakeLogFilename(name_prefix_, name_suffix_);
    fd_ = open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0666);
    if (fd_ < 0)
    {
        perror("open");
        assert (false);
    }
    bytes_written_ = 0;
}
