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

#include <cstdio>
#include <stdint.h>
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <sys/time.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <google/protobuf/repeated_field.h>

using namespace std;

#define AddBench(name, min_x, max_x) benchmark::AddBench("Test"#name, min_x, max_x, 0, 0, Test##name, NULL, NULL)
#define DeclareBench(name) void Test##name(benchmark::BenchmarkState& state)

static const size_t kMaxNumberSize = 1 << 10;

DeclareBench(sprintf)
{
    char buf[13];
    for (int i = 0; i != state.max_x; ++i)
    {
        int n = sprintf(buf, "%d", i);
        string s(buf, n);
    }
}

DeclareBench(stringstream)
{
    stringstream ss;
    for (int i = 0; i != state.max_x; ++i)
    {
        ss << i;
        string s(ss.str());
        ss.clear();
        ss.str("");
    }
}

int main()
{
    AddBench(sprintf, 1, 1048576);
    AddBench(stringstream, 1, 1048576);
    benchmark::ExecuteAll();
    return 0;
}
