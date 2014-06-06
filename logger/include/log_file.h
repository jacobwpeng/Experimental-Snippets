/*
 * =====================================================================================
 *
 *       Filename:  log_file.h
 *        Created:  06/01/14 17:59:41
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  file class for logging
 *
 * =====================================================================================
 */

#ifndef  __LOG_FILE_H__
#define  __LOG_FILE_H__

#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>

class LogFile
{
    public:
        LogFile(const std::string& prefix, const std::string& suffix, size_t rotate_size, bool thread_safe);
        ~LogFile();

        void Append(const char* buf, size_t len);

    private:
        void NonLockAppend(const char* buf, size_t len);
        void RotateFile();
        void Flush();
        std::string MakeFilename();

    private:
        FILE * fp_;
        const std::string name_prefix_;
        const std::string name_suffix_;
        const size_t rotate_size_;
        size_t bytes_written_;
        boost::scoped_ptr<boost::mutex> mutex_;
};

#endif   /* ----- #ifndef __LOG_FILE_H__  ----- */
