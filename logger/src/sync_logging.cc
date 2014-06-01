/*
 * =====================================================================================
 *
 *       Filename:  sync_logging.cc
 *        Created:  06/01/14 19:26:16
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "sync_logging.h"

#include <cassert>

SyncLogging* SyncLogging::instance_ = NULL;
boost::scoped_ptr<boost::mutex> SyncLogging::mutex_;

SyncLogging::SyncLogging()
    :used_(0)
{

}

SyncLogging::~SyncLogging()
{

}

void SyncLogging::Init(bool thread_safe)
{
    if (thread_safe)
    {
        mutex_.reset( new boost::mutex );
    }
}

SyncLogging* SyncLogging::Instance()
{
    if (instance_ == NULL)
    {
        instance_ = new SyncLogging;
    }
    return instance_;
}

void SyncLogging::Flush()
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

void SyncLogging::NonLockFlush()
{
    if (output_)
    {
        output_(buf_, used_);
    }
    used_ = 0u;
}

void SyncLogging::Write(const char* buf, size_t len)
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

void SyncLogging::NonLockWrite(const char* buf, size_t len)
{
    if (len > kMaxLogLength) return;            /* drop long log */

    if (used_ + len > SyncLogging::kBufferLength)
    {
        NonLockFlush();
        assert (used_ == 0);
    }
    assert (used_ + len <= SyncLogging::kBufferLength);
    memcpy(buf_ + used_, buf, len);
    used_ += len;
}
