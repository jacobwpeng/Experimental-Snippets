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

using namespace std;

void TestDeque(benchmark::BenchmarkState& state)
{
    deque<int> container;
    for (int i = 0; i != state.max_x; ++i)
    {
        container.push_back(i);
    }
}

void TestVector(benchmark::BenchmarkState& state)
{
    vector<int> container;
    for (int i = 0; i != state.max_x; ++i)
    {
        container.push_back(i);
    }
}

#define AddBench(name, min_x, max_x) benchmark::AddBench(#name, min_x, max_x, 0, 0, name, NULL, NULL)

int main()
{
    AddBench(TestDeque, 100, 1 << 22);
    AddBench(TestVector, 100, 1 << 22);
    benchmark::ExecuteAll();
    return 0;
}
