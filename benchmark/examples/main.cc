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

#include <stdint.h>
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <iostream>
#include <algorithm>
#include <sys/time.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <google/protobuf/repeated_field.h>

using namespace std;

#define AddBench(name, min_x, max_x) benchmark::AddBench("Test"#name, min_x, max_x, 0, 0, Test##name, NULL, NULL)
#define DeclareBench(name) void Test##name(benchmark::BenchmarkState& state)

static const size_t kMaxNumberSize = 1 << 10;

DeclareBench(PtrContainer)
{
    for (int i = 0; i != state.max_x; ++i)
    {
        boost::ptr_vector<size_t> ints;
        //vector<size_t> ints;
        for (size_t idx = 0; idx != kMaxNumberSize; ++idx) ints.push_back(new size_t(idx));
    }
}

DeclareBench(RepeatedPtrField)
{
    for (int i = 0; i != state.max_x; ++i)
    {
        google::protobuf::RepeatedPtrField<size_t> ints;
        for (size_t idx = 0; idx != kMaxNumberSize; ++idx) *ints.Add() = idx;
    }
}

int main()
{
    AddBench(RepeatedPtrField, 1, 500);
    AddBench(PtrContainer, 1, 500);
    benchmark::ExecuteAll();
    return 0;
}
