/*
 * =====================================================================================
 *
 *       Filename:  log_cfile.h
 *        Created:  06/06/14 17:04:53
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __LOG_CFILE_H__
#define  __LOG_CFILE_H__

#include "log_file.h"
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>

class LogCFile : public ILogFile, boost::noncopyable
{
    public:
        LogCFile(const std::string& prefix, const std::string& suffix, size_t rotate_size, bool thread_safe);
        virtual ~LogCFile();

        virtual void Append(const char* buf, size_t len);

    private:
        virtual void Flush();
        virtual void RotateFile();
        void NonLockAppend(const char* buf, size_t len);
        void NonLockFlush();

    private:
        FILE * fp_;
        const std::string name_prefix_;
        const std::string name_suffix_;
        const size_t rotate_size_;
        size_t bytes_written_;
        boost::scoped_ptr<boost::mutex> mutex_;
};

#endif   /* ----- #ifndef __LOG_CFILE_H__  ----- */
