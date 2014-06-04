/*
 * =====================================================================================
 *
 *       Filename:  logger.hpp
 *        Created:  06/02/14 09:28:22
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  template class for Logging in different ways
 *
 * =====================================================================================
 */

#ifndef  __LOGGER_HPP__
#define  __LOGGER_HPP__

#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include "log_formatter.h"
#include "sync_logger.h"
#include "async_logger.h"

enum LogLevel
{
    kLogError = 0,
    kLogWarning = 1,
    kLogInfo = 2,
    kLogDebug = 3,
    kLogLevelNum = 4
};

template<typename Impl>
class Logger : boost::noncopyable
{
    private:
        // inspired by glog
        struct Voidify
        {
            void operator&(std::ostream&)
            {
            }
        };

    public:
        typedef boost::function <void(const char* buf, size_t len)> OutputFunc;

    public:
        static Logger* Instance();
        static const char* LogLevelNames[kLogLevelNum];
        static void Init(bool thread_safe);

        ~Logger();
        void Flush(bool stop = false)
        {
            impl_.Flush(stop);
        }
        void Write(const char* buf, size_t len)
        {
            impl_.Write(buf, len);
        }
        void SetOutput(const OutputFunc& output)
        {
            impl_.SetOutput(output);
        }

        void SetLogLevel(LogLevel level) { level_ = level; }
        LogLevel Level() const { return level_; }

    static Voidify dummy;
    private:
        Logger()
            :level_(kLogDebug)
        {
        }

    private:
        static Logger* instance_;
        LogLevel level_;
        Impl impl_;
};
template<typename Impl>
typename Logger<Impl>::Voidify Logger<Impl>::dummy;

template<typename Impl>
Logger<Impl>* Logger<Impl>::instance_ = NULL;

template<typename Impl>
const char* Logger<Impl>::LogLevelNames[kLogLevelNum];

template<typename Impl>
Logger<Impl>* Logger<Impl>::Instance()
{
    if (Logger<Impl>::instance_ == NULL)
    {
        Logger<Impl>::instance_ = new Logger<Impl>();
    }
    return Logger<Impl>::instance_;
}

template<typename Impl>
void Logger<Impl>::Init(bool thread_safe)
{
    Logger<Impl>::Instance()->impl_.Init(thread_safe);
    Logger<Impl>::LogLevelNames[kLogError] = "ERROR";
    Logger<Impl>::LogLevelNames[kLogWarning] = "WARNING";
    Logger<Impl>::LogLevelNames[kLogInfo] = "INFO";
    Logger<Impl>::LogLevelNames[kLogDebug] = "DEBUG";
}

#ifdef USE_SYNC_LOGGER
#define LoggerType Logger<SyncLogger>
#else
#define LoggerType Logger<AsyncLogger>
#endif

#define LoggerInst LoggerType::Instance()
// inspired by glog
#define LOG_IF(level) \
    ( LoggerInst->Level() < level) ? (void)0 : \
    LoggerType::dummy & (LogFormatter(BasenameRetriever(__FILE__).basename, __FUNCTION__, __LINE__, LoggerType::LogLevelNames[level]).stream())
#define LOG_DEBUG LOG_IF(kLogDebug)
#define LOG_INFO LOG_IF(kLogInfo)
#define LOG_WARNING LOG_IF(kLogWarning)
#define LOG_ERROR LOG_IF(kLogError)

#endif   /* ----- #ifndef __LOGGER_HPP__  ----- */
