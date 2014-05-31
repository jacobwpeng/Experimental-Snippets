/*
 * =====================================================================================
 *
 *       Filename:  test.cc
 *        Created:  05/30/14 23:09:02
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <sstream>
#include <boost/thread/thread.hpp>

using namespace std;

void PrintPointer(const char* name, void * p)
{
    cout << name << " -> " << p << '\n';
}

const char* basefilename(const char* filename)
{
    unsigned pos = 0;
    unsigned where = 0;
    char c;
    while ( (c = filename[pos]) != '\0' )
    {
        if (c == '/') where = pos;
        ++pos;
    }
    if (where == 0) return filename;
    else return filename + where + 1;
}

class LoggerStreambuf : public streambuf
{
    public:
        int used()
        {
            return pptr() - pbase();
        }

    private:
        streambuf* setbuf (char* s, streamsize n)
        {
            setp(s, s+n);
            return this;
        }
};

class LoggerStream : public ostream
{
    public:
        LoggerStream()
        {
            rdbuf(&streambuf_);
        }

        LoggerStreambuf* streambuf()
        {
            return &streambuf_;
        }

    private:
        LoggerStreambuf streambuf_;
};

class Logger
{
    public:
        void Write(const char* buf, int len)
        {
#ifdef USE_SYS_WRITE
            if (write(fd_, buf, len) < 0)
            {
                perror("write");
            }
#else
            fwrite(buf, 1, len, fp_);
#endif
        }

        static Logger* Instance()
        {
            static Logger logger;
            return &logger;
        }
        
        static void Init(const char* logfilename)
        {
            logfilename_ = logfilename;
            (void)Logger::Instance();
        }

        ~Logger()
        {
#ifdef USE_SYS_WRITE
            close(fd_);
#else
            fclose(fp_);
#endif
        }

    private:
        Logger()
        {
#ifdef USE_SYS_WRITE
            fd_ = open(logfilename_, O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666);
            assert (fd_ > 0);
#else
            fp_ = fopen(logfilename_, "a");
            ftruncate(fileno(fp_), 0);
#endif
        }

    private:
        static const char* logfilename_;
#ifdef USE_SYS_WRITE
        int fd_;
#else
        FILE * fp_;
#endif
};

const char * Logger::logfilename_;

#define LoggerInst Logger::Instance()

static __thread time_t lasttime = 0;
static __thread struct tm tm;

class LoggerRecorder
{
    public:
        LoggerRecorder(const char* filename, int lineno)
        {
            header_len_ = FormatHeader(filename, lineno);
            stream_.rdbuf()->pubsetbuf(buf + header_len_, sizeof buf - header_len_);
        }

        ~LoggerRecorder()
        {
            int len = header_len_ + stream_.streambuf()->used();
            buf[len] = '\n';
            len += 1;
            LoggerInst->Write(buf, len);
        }

        ostream& stream() { return stream_; }

    private:
        int FormatHeader(const char* filename, int lineno)
        {
            struct timeval tv;

            int ret = gettimeofday(&tv, NULL);
            assert (ret == 0);
            (void)ret;
            if (tv.tv_sec != lasttime)
            {
                lasttime = tv.tv_sec;
                if (NULL != localtime_r(&tv.tv_sec, &tm))
                {
                    strftime(fmt, sizeof fmt, "[%Y-%m-%d %H:%M:%S.%%06u %%s:%%d][DEBUG] ", &tm);
                }
            }
            return snprintf(buf, sizeof(buf), fmt, tv.tv_usec, basefilename(filename), lineno);
        }

    private:
        LoggerStream stream_;
        int header_len_;
        char fmt[64];
        char buf[1024];
};

#define LOG_DEBUG (LoggerRecorder(__FILE__, __LINE__).stream())

void ThreadRoutine()
{
    for (int i = 0; i != 1000000; ++i)
    {
        LOG_DEBUG << "this just makes a long sentence, no one wants to see this msg, real part is thread id = " << boost::this_thread::get_id() << ", i = " << i;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) return -1;
    Logger::Init(argv[1]);

    boost::thread_group threads;

    for (int i = 0; i != 4; ++i)
    {
        threads.create_thread(ThreadRoutine);
    }

    threads.join_all();
    return 0;
}
