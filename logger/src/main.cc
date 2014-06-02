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

#include "log_file.h"
#include "sync_logging.h"

static LogFile * file = NULL;
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
}

void ThreadRoutine()
{
    for (int i = 0; i != kLogNum; ++i)
    {
        LOG_DEBUG << "this just makes a long sentence, no one wants to see this msg, real part is thread id = " << boost::this_thread::get_id() << ", i = " << i;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) return -1;

    file = new LogFile(argv[1], kRotateSize, true);
    SyncLoggingInst->Init(true);
    SyncLoggingInst->SetOutput(Output);

    boost::thread_group threads;
    uint64_t start = GetTimestamp();

    for (int i = 0; i != kThreadNum; ++i) threads.create_thread(ThreadRoutine);

    threads.join_all();
    SyncLoggingInst->Flush();
    uint64_t end = GetTimestamp();
    uint64_t delta = end - start;
    const double time_seconds = static_cast<double>(delta) / 1000000;

    printf("time: %.3f, Message: %.3f/s, Speed: %.3f/MiB\n", 
            time_seconds, static_cast<double>(kLogNum * kThreadNum)/time_seconds, static_cast<double>(gTotalBytes)/(time_seconds* 1048576));

    delete file;
    return 0;
}
