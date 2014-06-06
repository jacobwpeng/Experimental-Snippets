/*
 * =====================================================================================
 *
 *       Filename:  logsvrd.cc
 *        Created:  06/04/14 14:14:19
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "logsvrd.h"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

#include "log_fd_file.h"

sig_atomic_t LogServer::running_ = true;

void LogServer::Stop(int)
{
    LogServer::running_ = false;
}

LogServer::LogServer()
    :worker_num_(0)
{
}

LogServer::~LogServer()
{
    boost::interprocess::named_mutex::remove("logsvrd");
    boost::interprocess::named_condition::remove("logsvrd");
}

int LogServer::Init(const char* confpath)
{
    InitSyncTools();
    using namespace boost::property_tree;
    ptree pt;
    try
    {
        read_xml(confpath, pt, xml_parser::no_comments);
        process_bus_.reset (new ProcessBus());
        int ret = process_bus_->Init( pt.get_child("root.bus") );
        if (ret < 0)
        {
            fprintf(stderr, "bus init failed, ret = %d\n", ret);
            return ret;
        }
        ret = process_bus_->Listen();
        if (ret < 0)
        {
            fprintf(stderr, "bus listen failed, ret = %d\n", ret);
            return ret;
        }
        std::string logpath = pt.get<std::string>("root.log.<xmlattr>.path");
        prefix_ = logpath + "worker";

        worker_num_ = pt.get<unsigned>("root.logsvrd.<xmlattr>.workers");
    }catch(ptree_error& e)
    {
        fprintf(stderr, "ptree_error, msg[%s]\n", e.what());
        return -1;
    }
    return 0;
}

void LogServer::Run()
{
    daemon(0, 0);
    if (worker_num_ == 0)
    {
        WorkerRoutine();
    }
    else
    {
        for (unsigned i = 0; i != worker_num_; ++i)
        {
            if (fork() == 0)
            {
                /* worker */
                WorkerRoutine();
                break;
            }
            else if (i + 1 == worker_num_)
            {
                MonitorRoutine();
            }
        }
    }
}

void LogServer::InitSyncTools()
{
    mutex_.reset (new MutexType(boost::interprocess::open_or_create_t(), "logsvrd"));
    cond_.reset (new ConditionType(boost::interprocess::open_or_create_t(), "logsvrd"));
}

void LogServer::MonitorRoutine()
{
    while (running_)
    {
        int status;
        if (::wait(&status) < 0)
        {
            perror("wait");
        }
        else if (not WIFEXITED(status))
        {
            /* worker exit unexpectedly */
            if (fork() == 0)
            {
                WorkerRoutine();
            }
        }
        else
        {
            /* normal exit */
        }
    }
}

void LogServer::WorkerRoutine()
{
    signal(SIGINT, LogServer::Stop);
    signal(SIGTERM, LogServer::Stop);
    signal(SIGQUIT, LogServer::Stop);
    assert (process_bus_);

    std::vector<std::string> msgs;
    char pidbuf[12];
    snprintf(pidbuf, sizeof pidbuf, "%d", getpid());
    boost::scoped_ptr<LogFdFile> file( new LogFdFile(prefix_, pidbuf, 1 << 30, false) );
    const unsigned kMaxMessageEachTime = 1 << 10;
    using std::cerr;
    unsigned msg_count = 0;
    while (running_)
    {
        {
            boost::interprocess::scoped_lock<MutexType> lock(*mutex_);
            while (process_bus_->size() == 0 and running_)
            {
                boost::posix_time::ptime flush_interval = boost::posix_time::from_time_t( time(NULL) + 1);
                cond_->timed_wait(lock, flush_interval);
                cerr << "Wakeup\n";
            }

            unsigned num = 0;
            while (num < kMaxMessageEachTime)
            {
                ProcessBus::BusElement e = process_bus_->Read();
                if (e.len == 0) break;

                msgs.push_back(std::string(e.buf, e.len));
                ++num;
            }
        }
        {
            for (size_t i = 0; i != msgs.size(); ++i)
            {
                file->Append(msgs[i].data(), msgs[i].size());
                ++msg_count;
            }
            msgs.clear();
        }
    }
    cerr << msg_count << '\n';
}
