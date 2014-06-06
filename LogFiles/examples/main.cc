/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  06/06/14 16:19:40
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "log_fd_file.h"
#include "log_cfile.h"
#include "benchmark.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

void TestAppendFile(benchmark::BenchmarkState& state, ILogFile* file, const std::string& msg)
{
    for (int x = 0; x < state.max_x; ++x)
    {
        file->Append(msg.data(), msg.size());
    }
}

int main()
{
    std::string msg("Perror produces a message on the standard error output, describing the last error encountered during a call to a system or library function.\n");

    boost::shared_ptr<ILogFile> file( new LogFdFile("/tmp/LogFdFile", "", 1 << 30, false));
    benchmark::AddBench("TestFdFile", 1, 1 << 20, 0, 0, boost::bind(TestAppendFile, _1, file.get(), msg), NULL, NULL);

    boost::shared_ptr<ILogFile> cfile( new LogCFile("/tmp/LogCFile", "", 1 << 30, false));
    benchmark::AddBench("TestCFile", 1, 1 << 20, 0, 0, boost::bind(TestAppendFile, _1, cfile.get(), msg), NULL, NULL);

    benchmark::ExecuteAll();
    return EXIT_SUCCESS;
}
