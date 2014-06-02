/*
 * =====================================================================================
 *
 *       Filename:  async_logger.h
 *        Created:  06/02/14 11:05:20
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  use LogThread to do Async Logging
 *
 * =====================================================================================
 */

#ifndef  __ASYNC_LOGGER_H__
#define  __ASYNC_LOGGER_H__

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "fixed_size_buffer.hpp"

class AsyncLogger
{
    private:
        typedef boost::function <void(const char* buf, size_t len)> OutputFunc;

    public:
        AsyncLogger();
        ~AsyncLogger();

        void Init(bool thread_safe);
        void Flush(bool stop = false);
        void Write(const char* buf, size_t len);
        void SetOutput(const OutputFunc& output);

    private:
        static const size_t kBufferSize = 1 << 16;
        static const unsigned kFlushInterval;      /* ms */
        typedef FixedSizeBuffer<kBufferSize> FixedSizeBufferType;
        typedef boost::shared_ptr<FixedSizeBufferType> FixedSizeBufferPtr;;

    private:
        void LogTheadRoutine();

    private:
        OutputFunc output_;
        std::vector<FixedSizeBufferPtr> buffers_;
        FixedSizeBufferPtr  current_, next_;
        boost::mutex mutex_;
        boost::condition_variable cond_;
        bool running_;
        boost::scoped_ptr<boost::thread> thread_;
};

#endif   /* ----- #ifndef __ASYNC_LOGGER_H__  ----- */
