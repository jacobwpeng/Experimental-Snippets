/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  06/06/14 10:03:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "benchmark.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <vector>
#include <string>
#include <iostream>
#include <boost/scoped_ptr.hpp>

using namespace std;
typedef vector<string> StringList;

static string msg;
static int fd;
static const char* filename = "/tmp/test.benchmark";
const size_t kMaxBufferLength = 1 << 12;

void TestWrite(benchmark::BenchmarkState& state)
{
    for (int y = 0; y < state.max_y; ++y)
    {
        write(fd, msg.data(), state.max_x); 
    }
}

void TestWritev(benchmark::BenchmarkState& state)
{
    struct iovec * iov;
    iov = (iovec*) alloca(state.max_y * sizeof (struct iovec) );
    for (int y = 0; y < state.max_y; ++y)
    {
        iov[y].iov_base = (void*)msg.data();
        iov[y].iov_len = state.max_x;
    }
    writev(fd, iov, state.max_y);
}

int main()
{
    msg = string(kMaxBufferLength-1, ' ');
    msg += '\n';
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    assert (fd >= 0);

    /* x -> buffer length, y -> iov length */
    benchmark::AddBench("TestWrite", 50, kMaxBufferLength, 1, IOV_MAX, TestWrite, NULL, NULL);
    benchmark::AddBench("TestWritev", 50, kMaxBufferLength, 1, IOV_MAX, TestWritev, NULL, NULL);
    benchmark::ExecuteAll();
    close(fd);
}
