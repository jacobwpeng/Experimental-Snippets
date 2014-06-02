/*
 * =====================================================================================
 *
 *       Filename:  async_logger.cc
 *        Created:  06/02/14 11:09:33
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  use LogThread to do Async Logging
 *
 * =====================================================================================
 */

#include "async_logger.h"

#include <cstdio>
#include <boost/atomic.hpp>
#include <boost/bind.hpp>

static boost::atomic<bool> running_(false);
const unsigned AsyncLogger::kFlushInterval = 1000;      /* ms */
static size_t max_buffers_size = 0u;

AsyncLogger::AsyncLogger()
    :current_(new FixedSizeBufferType), next_(new FixedSizeBufferType), running_(true)
{

}

AsyncLogger::~AsyncLogger()
{

}

void AsyncLogger::Init(bool thread_safe)
{
    printf("AsyncLogger::Init(%s)\n", thread_safe ? "true" : "false");
    thread_.reset( new boost::thread( boost::bind(&AsyncLogger::LogTheadRoutine, this)));
    /* fix me : wait thread_ notification */
}

void AsyncLogger::Flush(bool stop)
{
    if (stop)
    {
        running_ = false;
        cond_.notify_all();
        thread_->join();
    }
    else
    {
        cond_.notify_all();
    }
}

void AsyncLogger::Write(const char* buf, size_t len)
{
    boost::mutex::scoped_lock lock(mutex_);
    if (current_->Available() >= len)
    {
        current_->Append(buf, len);
    }
    else
    {
        buffers_.push_back(current_);
        if (next_)
        {
            current_ = next_;
            next_.reset();
        }
        else
        {
            current_.reset( new FixedSizeBufferType() );
        }
        assert (current_->Available() >= len);
        current_->Append(buf, len);
        cond_.notify_all();
    }
}

void AsyncLogger::SetOutput(const OutputFunc& output)
{
    output_ = output;
}

void AsyncLogger::LogTheadRoutine()
{
    std::vector<FixedSizeBufferPtr> buffers_to_write;
    FixedSizeBufferPtr tmp1( new FixedSizeBufferType() );
    boost::chrono::milliseconds interval(AsyncLogger::kFlushInterval);

    while (running_)
    {
        assert (tmp1->used() == 0);
        {
            boost::unique_lock<boost::mutex> lock(mutex_);
            while (current_->Available() == AsyncLogger::kBufferSize and buffers_.empty()) cond_.wait_for(lock, interval);

            if (not buffers_.empty()) 
            {
                if (max_buffers_size < buffers_.size()) max_buffers_size = buffers_.size();
                buffers_to_write.swap(buffers_);
            }

            if (not next_)
            {
                next_ = tmp1;
                tmp1.reset();
            }
        }
        {
            for (size_t i = 0; i != buffers_to_write.size(); ++i)
            {
                if (output_) output_(buffers_to_write[i]->buf(), buffers_to_write[i]->used());
            }

            if (tmp1 == NULL)
            {
                assert (buffers_to_write.empty() == false);
                tmp1 = buffers_to_write[0];
                tmp1->ClearUsed();
            }
            if (not buffers_to_write.empty())
            {
                std::vector<FixedSizeBufferPtr> tmp;
                tmp.swap(buffers_to_write);
            }
        }
    }

    /* flush all buffered */
    boost::mutex::scoped_lock lock(mutex_);
    if (max_buffers_size < buffers_.size()) max_buffers_size = buffers_.size();
    for (size_t i = 0; i != buffers_.size(); ++i)
    {
        output_(buffers_[i]->buf(), buffers_[i]->used());
    }
    if (current_->used() != 0u)
    {
        output_(current_->buf(), current_->used());
    }
    if (next_ and next_->used() != 0u)
    {
        output_(next_->buf(), next_->used());
    }
}
