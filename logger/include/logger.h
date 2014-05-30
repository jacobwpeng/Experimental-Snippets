/*
 * =====================================================================================
 *
 *       Filename:  logger.h
 *        Created:  05/29/14 22:13:57
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __LOGGER__
#define  __LOGGER__

#include <string>

namespace detail
{
    class LogStream;
}

class Logger
{
    public:
        enum LogLevel
        {
            kLogError = 0,
            kLogWarning = 1,
            kLogInfo = 2,
            kLogDebug = 3,
            kLogMax
        };
        static const unsigned kMaxLineLength = 4095;
        static const char * LogLevelName[kLogMax];

    public:
        ~Logger();

        void Write(const char* buf, unsigned len);
        static Logger* Instance();
        static void Init();

    private:
        Logger(); 
        void FormatHeader();

};

static __thread char buf[Logger::kMaxLineLength];
static __thread char fmt[64];
static __thread unsigned len = 0;

namespace detail
{
    class LogStream
    {
        public:
            LogStream(Logger::LogLevel level, const char* filename, int line);
            ~LogStream();

        private:
            void FormatHeader();

        private:
            Logger::LogLevel level_;
            const char* filename_;
            int line_;
    };
}

#define LOG_DEBUG (detail::LogStream(Logger::kLogDebug, __FILE__, __LINE__))
#define LOG(name) LOG_##name

#endif   /* ----- #ifndef __LOGGER__  ----- */
