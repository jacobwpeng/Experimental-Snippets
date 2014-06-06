/*
 * =====================================================================================
 *
 *       Filename:  benchmark.cc
 *        Created:  06/06/14 10:02:43
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "benchmark.h"

#include <sys/time.h>
#include <cassert>
#include <cstdio>
#include <iostream>
#include <boost/foreach.hpp>

namespace detail
{
    bool needs_header = true;
    void GenerateRange(int min, int max, std::vector<int>* range)
    {
        if (min == 0) min = 1;
        assert (min <= max);
        assert (range);
        for (int i = min; i < max; i *= 2)
        {
            range->push_back(i);
        }
        if (min != max) range->push_back(max);
    }

    uint64_t GetTimestamp()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
    }

    void OutputBenchmark(const std::string& name, int x, int y, int iteration, uint64_t duration)
    {
        const size_t max_name_length = 19;
        if (needs_header)
        {
            std::cout << std::string(max_name_length + 1, ' ');
            printf("           x               y       iteration(100us)          speed   (iteration/s)\n");
            needs_header = false;
        }
        if (name.size() >= max_name_length)
        {
            std::cout << std::string(name.c_str(), max_name_length);
            std::cout << ':';
        }
        else
        {
            size_t blank = max_name_length - name.size();
            std::cout << name << ':' << std::string(blank, ' ');
        }
        printf("%12d    %12d    %12d    %20.3f\n", x, y, iteration, static_cast<double>(iteration)/duration * 1000000);
    }
}

namespace benchmark
{

/*-----------------------------------------------------------------------------
 *  BenchmarkState
 *-----------------------------------------------------------------------------*/
BenchmarkState::BenchmarkState(int max_x, int max_y)
    :max_x(max_x), max_y(max_y), duration(0), data(NULL)
{
}


/*-----------------------------------------------------------------------------
 *  BenchmarkSuite
 *-----------------------------------------------------------------------------*/
BenchmarkSuite::BenchmarkSuite(const std::string& name, int min_x, int max_x, int min_y, int max_y, 
        const BenchmarkFunc& bench, const BenchmarkFunc& setup, const BenchmarkFunc& teardown)
    :name(name), bench(bench), setup(setup), teardown(teardown)
{
    detail::GenerateRange(min_x, max_x, &xs);
    detail::GenerateRange(min_y, max_y, &ys);
}

void BenchmarkSuite::Execute()
{
    const uint64_t max_duration = 1000 * 100; /* us */
    BOOST_FOREACH(int x, xs)
    {
        BOOST_FOREACH(int y, ys)
        {
            uint64_t duration = 0;
            int iteration = 0;
            while (duration < max_duration)
            {
                BenchmarkState state(x, y);
                if (setup) setup(state);
                uint64_t start = detail::GetTimestamp();
                bench(state);
                uint64_t end = detail::GetTimestamp();
                if (teardown) teardown(state);
                state.duration = end - start;
                duration += state.duration;
                ++iteration;
            }
            detail::OutputBenchmark(name, x, y, iteration, duration);
        }
    }
}

/*-----------------------------------------------------------------------------
 *  BenchmarkMonitor
 *-----------------------------------------------------------------------------*/
BenchmarkMonitor* BenchmarkMonitor::Instance()
{
    static BenchmarkMonitor monitor;
    return &monitor;
}

BenchmarkMonitor::BenchmarkMonitor()
{
}

BenchmarkMonitor::~BenchmarkMonitor()
{
}

void BenchmarkMonitor::Add(const BenchmarkSuite& suite)
{
    suites_.push_back(suite);
}

void BenchmarkMonitor::Execute()
{
    for (size_t i = 0; i != suites_.size(); ++i)
    {
        suites_[i].Execute();
    }
}

/*-----------------------------------------------------------------------------
 *  global functions
 *-----------------------------------------------------------------------------*/

void AddBench(const std::string& name, int min_x, int max_x, int min_y, int max_y, 
        const BenchmarkFunc& bench, const BenchmarkFunc& setup, const BenchmarkFunc& teardown)
{
    BenchmarkMonitor::Instance()->Add(BenchmarkSuite(name, min_x, max_x, min_y, max_y, bench, setup, teardown));
}

void ExecuteAll()
{
    BenchmarkMonitor::Instance()->Execute();
}

} /* namespace benchmakr */
