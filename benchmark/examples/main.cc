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

#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/scoped_ptr.hpp>

#include "userinfo.pb.h"
#include "log_stream.h"

using namespace std;

char buf[1 << 20];
int i32 = 1;
int64_t i64 = 2;
int si32 = 3;
int64_t si64 = 4;
int sf32 = 5;
int64_t sf64 = 6;
unsigned u32 = 7;
uint64_t u64 = 8;
double d = 3.14159;

const char * LogMessage = "The usleep() function suspends execution of the calling thread for (at least) usec microseconds.  "
"The sleep may be lengthened slightly by any system activity or by the time spent processing the call or by the granularity of system timers.";

struct User
{
    int i32;
    int64_t i64;
    int si32;
    int64_t si64;
    int sf32;
    int64_t sf64;
    unsigned u32;
    uint64_t u64;
    double d;
    char buf[1024];
    char buf1[1024];
    char buf2[1024];
    char buf3[1024];
    char buf4[1024];
    char buf5[1024];
    char buf6[1024];
    char buf7[1024];
    char buf8[1024];
    char buf9[1024];
};

struct Base
{
    virtual void Func(){}
};

struct Derived : Base
{
    virtual void Func(){}
};

void TestStruct(benchmark::BenchmarkState& state)
{
    for (int x = 0; x < state.max_x; ++x)
    {
        User user;
        user.i32 = i32;
        user.i64 = i64;
        user.si32 = si32;
        user.si64 = si64;
        user.sf32 = sf32;
        user.sf64 = sf64;
        user.u32 = u32;
        user.u64 = u64;
        user.d = d;

        memcpy(buf, &user, sizeof(user));
    }
}

void TestProtobuf(benchmark::BenchmarkState& state)
{
    for (int x = 0; x < state.max_x; ++x)
    {
        Userinfo user;
        user.set_i32(i32);
        user.set_i64(i64);
        user.set_si32(si32);
        user.set_si64(si64);
        user.set_sf32(sf32);
        user.set_sf64(sf64);
        user.set_u32(u32);
        user.set_u64(u64);
        user.set_d(d);
        user.set_s(string(1024, ' '));
        user.set_s1(string(1024, ' '));
        user.set_s2(string(1024, ' '));
        user.set_s3(string(1024, ' '));
        user.set_s4(string(1024, ' '));
        user.set_s5(string(1024, ' '));
        user.set_s6(string(1024, ' '));
        user.set_s7(string(1024, ' '));
        user.set_s8(string(1024, ' '));
        user.set_s9(string(1024, ' '));

        user.SerializeToArray(buf, sizeof(buf));
    }
}

void TestOstringstream(benchmark::BenchmarkState& state)
{
    ostringstream oss;
    for (int x = 0; x < state.max_x; ++x)
    {
        oss << LogMessage;
        oss.str("");
    }
}

void TestStreambuf(benchmark::BenchmarkState& state)
{
    char buf[4096];
    LogStream logstream;
    for (int x = 0; x < state.max_x; ++x)
    {
        logstream.rdbuf()->pubsetbuf(buf, sizeof(buf));
        logstream << LogMessage;
        (void)dynamic_cast<LogStreambuf*>(logstream.rdbuf());
    }
}

void TestBaseVirtualCall(benchmark::BenchmarkState& state)
{
    Base * inst = new Derived;
    for (int x = 0; x < state.max_x; ++x) inst->Func();
}

void TestChildVirtualCall(benchmark::BenchmarkState& state)
{
    Derived * inst = new Derived;
    for (int x = 0; x < state.max_x; ++x) inst->Func();
}

void TestDirectCall(benchmark::BenchmarkState& state)
{
    Derived inst;
    for (int x = 0; x < state.max_x; ++x) inst.Func();
}

#define AddBench(name) benchmark::AddBench(#name, 50, 1<<22, 0, 0, name, NULL, NULL)

int main()
{
    AddBench(TestDirectCall);
    AddBench(TestBaseVirtualCall);
    AddBench(TestChildVirtualCall);
    //benchmark::AddBench("TestVirtualCall", 50, 1 << 20, 0, 0, TestVirtualCall, NULL, NULL);
    //benchmark::AddBench("TestOstringstream", 50, 1 << 20, 0, 0, TestOstringstream, NULL, NULL);
    //benchmark::AddBench("TestLogStream", 50, 1 << 20, 0, 0, TestStreambuf, NULL, NULL);
    //benchmark::AddBench("TestStruct", 50, 1 << 22, 0, 0, TestStruct, NULL, NULL);
    //benchmark::AddBench("TestProtobuf", 50, 1 << 22, 0, 0, TestProtobuf, NULL, NULL);
    //benchmark::AddBench("TestLengthDelimitedString", 50, 1 << 22, 0, 0, TestLengthDelimitedString, NULL, NULL);
    benchmark::ExecuteAll();
}
