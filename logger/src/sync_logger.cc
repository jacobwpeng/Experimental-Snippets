/*
 * =====================================================================================
 *
 *       Filename:  sync_logger.cc
 *        Created:  06/01/14 19:26:16
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "sync_logger.h"

#include <cassert>
#include <cstdio>

SyncLogger::SyncLogger()
//    :used_(0)
{
}

SyncLogger::~SyncLogger()
{
}

void SyncLogger::Init(bool thread_safe)
{
    printf("SyncLogger::Init(%s)\n", thread_safe ? "true" : "false");
    if (thread_safe)
    {
        mutex_.reset(new boost::mutex);
    }
}

void SyncLogger::Write(const char* buf, size_t len)
{
    if (mutex_)
    {
        boost::mutex::scoped_lock lock(*mutex_);
        NonLockWrite(buf, len);
    }
    else
    {
        NonLockWrite(buf, len);
    }
}

void SyncLogger::Flush(bool)
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

void SyncLogger::NonLockWrite(const char* buf, size_t len)
{
    if (len > kMaxLogLength) return;            /* drop long log */

    //if (used_ + len > SyncLogger::kBufferLength)
    //{
    //    NonLockFlush();
    //    assert (used_ == 0);
    //}
    //assert (used_ + len <= SyncLogger::kBufferLength);
    //memcpy(buf_ + used_, buf, len);
    //used_ += len;
    output_(buf, len);
}

void SyncLogger::NonLockFlush()
{
    //if (output_)
    //{
    //    output_(buf_, used_);
    //}
    //used_ = 0u;
}
