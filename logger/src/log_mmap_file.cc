/*
 * =====================================================================================
 *
 *       Filename:  log_mmap_file.cc
 *        Created:  06/03/14 19:44:49
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "log_mmap_file.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cstdio>

LogMMapFile::LogMMapFile(const std::string& logname, size_t rotate_size, bool thread_safe)
    :fd_(0), logname_(logname), rotate_size_(rotate_size), bytes_written_(0),
    mutex_(thread_safe ? new boost::mutex() : NULL), mem_(NULL)
{
    std::string filename = MakeFilename();
    fd_ = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
    assert (fd_ >= 0);

    MakeMMap();
}

LogMMapFile::~LogMMapFile()
{
    if (fd_)
    {
        int ret = munmap(mem_, rotate_size_);
        if (ret < 0)
        {
            perror("munmap");
        }
        ftruncate(fd_, bytes_written_);
        close(fd_);
    }
}

void LogMMapFile::Append(const char* buf, size_t len)
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

void LogMMapFile::NonLockAppend(const char* buf, size_t len)
{
    if (bytes_written_ + len > rotate_size_)
    {
        RotateFile();
        bytes_written_ = 0u;
    }
    memcpy(mem_ + bytes_written_, buf, len);
    bytes_written_ += len;
    //int bytes = write(fd_, buf, len);
    //assert (bytes >= 0);
    //bytes_written_ += bytes;
}

void LogMMapFile::Flush()
{
    /* no-op */
}

void LogMMapFile::MakeMMap()
{
    assert (fd_ >= 0);
    ftruncate(fd_, rotate_size_);
    void * mem = ::mmap(NULL, rotate_size_, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS , fd_, 0);
    if (mem == MAP_FAILED)
    {
        perror("mmap");
    }
    mem_ = (char*)mem;
}

std::string LogMMapFile::MakeFilename()
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

void LogMMapFile::RotateFile()
{
    assert (fd_ != 0);
    assert (mem_ != NULL);
    close(fd_);
    int ret = munmap(mem_, rotate_size_);
    if (ret < 0)
    {
        perror("munmap");
    }

    std::string filename = MakeFilename();
    fd_ = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
    assert (fd_ >= 0);

    MakeMMap();
}
