/*
 * =====================================================================================
 *
 *       Filename:  log_mmap_file.h
 *        Created:  06/03/14 19:43:31
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  log file use mmap
 *
 * =====================================================================================
 */

#ifndef  __LOG_MMAP_FILE_H__
#define  __LOG_MMAP_FILE_H__

#include <string>
#include <boost/thread/mutex.hpp>
#include <boost/scoped_ptr.hpp>

class LogMMapFile
{
    public:
        LogMMapFile(const std::string& logname, size_t rotate_size, bool thread_safe);
        ~LogMMapFile();

        void Append(const char* buf, size_t len);

    private:
        void NonLockAppend(const char* buf, size_t len);
        void RotateFile();
        void Flush();
        void MakeMMap();
        std::string MakeFilename();

    private:
        int fd_;
        const std::string logname_;
        const size_t rotate_size_;
        size_t bytes_written_;
        boost::scoped_ptr<boost::mutex> mutex_;
        char * mem_;
};

#endif   /* ----- #ifndef __LOG_MMAP_FILE_H__  ----- */
