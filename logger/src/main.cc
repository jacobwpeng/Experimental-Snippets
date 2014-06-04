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

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <cstdio>
#include <boost/thread/thread.hpp>
#include <boost/pool/pool.hpp>

#include "logger.hpp"
#include "log_file.h"
#include "log_mmap_file.h"

typedef LogFile LogFileType ;
static LogFileType * file = NULL;
static const size_t kLogNum = 1000000;
static const size_t kThreadNum = 4;
static const size_t kRotateSize = 100 * 1 << 20; /* 100 MiB */

static size_t gTotalBytes = 0;

uint64_t GetTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void Output(const char* buf, size_t len)
{
    gTotalBytes += len;
    file->Append(buf, len);
    //fwrite(buf, 1, len, stderr);
}

void ThreadRoutine()
{
    std::string msg("Vim provides many ways of moving around within a document as well as commands for jumping between buffers.");
    for (int i = 0; i != kLogNum; ++i)
    {
    //    Output(msg.data(), msg.size());
        LOG_ERROR << msg;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) return -1;

#ifndef USE_SYNC_LOGGER
    file = new LogFileType(argv[1], kRotateSize, false);
#else
    file = new LogFileType(argv[1], kRotateSize, true);
#endif
    LoggerInst->Init(true);
    LoggerInst->SetOutput(Output);

    uint64_t start = GetTimestamp();
    //ThreadRoutine();
    boost::thread_group threads;
    for (int i = 0; i != kThreadNum; ++i) threads.create_thread(ThreadRoutine);
    threads.join_all();
    LoggerInst->Flush(true);
    uint64_t end = GetTimestamp();
    uint64_t delta = end - start;
    const double time_seconds = static_cast<double>(delta) / 1000000;

    printf("time: %.3f, Message: %.3f/s, Speed: %.3f/MiB\n", 
            time_seconds, static_cast<double>(kLogNum * kThreadNum)/time_seconds, static_cast<double>(gTotalBytes)/(time_seconds* 1048576));

    delete file;
    return 0;
}
