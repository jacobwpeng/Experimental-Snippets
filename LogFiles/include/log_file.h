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

class ILogFile
{
    public:
        virtual void Append(const char* buf, size_t len) = 0;
        virtual ~ILogFile() {}

    private:
        virtual void Flush() = 0;
        virtual void RotateFile() = 0;
};

std::string MakeLogFilename(const std::string& prefix, const std::string& suffix);

#endif   /* ----- #ifndef __LOG_FILE_H__  ----- */
