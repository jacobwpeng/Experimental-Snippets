/*
 * =====================================================================================
 *
 *       Filename:  benchmark.h
 *        Created:  06/06/14 10:01:38
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __BENCHMARK_H__
#define  __BENCHMARK_H__

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace benchmark
{

    struct BenchmarkState : boost::noncopyable
    {
        BenchmarkState(int max_x, int max_y);
        const int max_x;
        const int max_y;

        uint64_t duration;
        void * data;
    };

    typedef boost::function<void(BenchmarkState&)> BenchmarkFunc;

    struct BenchmarkSuite
    {
        BenchmarkSuite(const std::string& name, int min_x, int max_x, int min_y, int max_y, 
                const BenchmarkFunc&, const BenchmarkFunc& , const BenchmarkFunc& );
        void Execute();

        std::string name;
        std::vector<int> xs;
        std::vector<int> ys;
        BenchmarkFunc bench;
        BenchmarkFunc setup;
        BenchmarkFunc teardown;
    };

    class BenchmarkMonitor : boost::noncopyable
    {
        public:
            static BenchmarkMonitor* Instance();

            ~BenchmarkMonitor();
            void Add(const BenchmarkSuite& );
            void Execute();

        private:
            BenchmarkMonitor();

        private:
            std::vector<BenchmarkSuite> suites_;
    };

    void AddBench(const std::string& name, int min_x, int max_x, int min_y, int max_y, 
            const BenchmarkFunc&, const BenchmarkFunc&, const BenchmarkFunc& );

    void AddSimpleBench(const std::string& name, int min_x, int max_x, const BenchmarkFunc&);

    void ExecuteAll();
}

#endif   /* ----- #ifndef __BENCHMARK_H__  ----- */
