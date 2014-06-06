/*
 * =====================================================================================
 *
 *       Filename:  client.cc
 *        Created:  06/04/14 14:53:39
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fstream>
#include <iostream>
#include <boost/atomic.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include "process_bus.h"

using namespace std;

typedef boost::interprocess::named_mutex MutexType;
static boost::shared_ptr<MutexType> mutex;
typedef boost::interprocess::named_condition ConditionType;
static boost::scoped_ptr<ConditionType> cond;
const unsigned kLoopNum = 1000000;

static bool running = true;
static boost::scoped_ptr<ProcessBus> bus;
static boost::atomic<int> failed(0);

static size_t gTotalBytes = 0;
uint64_t GetTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}

void Stop(int)
{
    running = false;
}

void InitSyncTools()
{
    mutex.reset (new MutexType(boost::interprocess::open_only_t(), "logsvrd"));
    cond.reset (new ConditionType(boost::interprocess::open_only_t(), "logsvrd"));
}

int InitProcessBus(const char* confpath)
{
    using namespace boost::property_tree;
    ptree pt;
    try
    {
        read_xml(confpath, pt, xml_parser::no_comments);
        bus.reset (new ProcessBus());
        int ret = bus->Init( pt.get_child("root.bus") );
        if (ret < 0)
        {
            fprintf(stderr, "bus init failed, ret = %d\n", ret);
            return ret;
        }
        ret = bus->Connect();
        if (ret < 0)
        {
            fprintf(stderr, "bus connect failed, ret = %d\n", ret);
            return ret;
        }
    }catch(ptree_error& e)
    {
        fprintf(stderr, "ptree_error, msg[%s]\n", e.what());
        return -1;
    }
    return 0;
}

void Output(const char* buf, int len)
{
    ProcessBus::BusElement e;
    e.buf = buf;
    e.len = len;
    {
        boost::interprocess::scoped_lock<MutexType> lock(*mutex);
        if (false == bus->Write(e))
        {
            ++failed;
            return;
        }
        if (bus->size() % 10 == 0)
        {
            cond->notify_one();
        }
    }
    gTotalBytes += len;
}

void ThreadRoutine()
{
    string msg("Increments the semaphore count. If there are processes/threads blocked waiting for the semaphore\n");
    for (unsigned i = 0; i != kLoopNum and running; ++i)
    {
        Output(msg.data(), msg.size());
    }
}

int main(int argc, char * argv[])
{
    if (argc != 2) return -1;

    InitProcessBus(argv[1]);
    assert (bus);
    InitSyncTools();
    signal(SIGINT, Stop);

    cout << "bus size: " << bus->size() << '\n';
    uint64_t start = GetTimestamp();
    ThreadRoutine();
    //boost::thread_group threads;
    //for (int i = 0; i != 4; ++i)
    //{
    //    threads.create_thread (ThreadRoutine);
    //}
    //threads.join_all();
    uint64_t end = GetTimestamp();
    uint64_t delta = end - start;
    const double time_seconds = static_cast<double>(delta) / 1000000;

    cout << "bus size: " << bus->size() << '\n';
    printf("failed: %d, time: %.3f, Message: %.3f/s, Speed: %.3f/MiB\n", failed.load(), 
            time_seconds, static_cast<double>(kLoopNum)/time_seconds, static_cast<double>(gTotalBytes)/(time_seconds* 1048576));

    return 0;
}
