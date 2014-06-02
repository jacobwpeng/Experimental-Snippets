/*
 * =====================================================================================
 *
 *       Filename:  sync_logger.h
 *        Created:  06/01/14 19:19:31
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __SYNC_LOGGER_H__
#define  __SYNC_LOGGER_H__

#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

class SyncLogger : boost::noncopyable
{
    public:
        typedef boost::function <void(const char* buf, size_t len)> OutputFunc;

    public:
        SyncLogger();
        ~SyncLogger();

        void Init(bool thread_safe);
        void SetOutput(const OutputFunc& output) { output_ = output; }
        void Flush(bool);
        void Write(const char* buf, size_t len);

    private:
        void NonLockWrite(const char* buf, size_t len);
        void NonLockFlush();

    private:
        static const size_t kBufferLength = 1 << 16;
        static const size_t kMaxLogLength = 1 << 12;
        boost::scoped_ptr<boost::mutex> mutex_;
        OutputFunc output_;
        char buf_[SyncLogger::kBufferLength];
        size_t used_;
};

#endif   /* ----- #ifndef __SYNC_LOGGER_H__  ----- */
