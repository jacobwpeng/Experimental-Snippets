/*
 * =====================================================================================
 *
 *       Filename:  sync_logging.h
 *        Created:  06/01/14 19:19:31
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __SYNC_LOGGING_H__
#define  __SYNC_LOGGING_H__

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "log_formatter.h"

class SyncLogging : boost::noncopyable
{
    public:
        typedef boost::function <void(const char* buf, size_t len)> OutputFunc;
        void Flush();

    public:
        static SyncLogging* Instance();
        ~SyncLogging();

        static void Init(bool thread_safe);

        void Write(const char* buf, size_t len);
        void SetOutput(const OutputFunc& output) { output_ = output; }

    private:
        SyncLogging();
        void NonLockWrite(const char* buf, size_t len);
        void NonLockFlush();

    private:
        static SyncLogging * instance_;
        static const size_t kBufferLength = 1 << 16;
        static const size_t kMaxLogLength = 1 << 12;
        static boost::scoped_ptr<boost::mutex> mutex_;
        OutputFunc output_;
        char buf_[SyncLogging::kBufferLength];
        size_t used_;
};

#define SyncLoggingInst SyncLogging::Instance()

#define LOG_DEBUG (LogFormatter(BasenameRetriever(__FILE__).basename, __LINE__).stream())

#endif   /* ----- #ifndef __SYNC_LOGGING_H__  ----- */
