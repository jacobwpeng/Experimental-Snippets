/*
 * =====================================================================================
 *
 *       Filename:  logsvrd.h
 *        Created:  06/04/14 14:13:42
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  log server
 *
 * =====================================================================================
 */

#ifndef  __LOGSVRD_H__
#define  __LOGSVRD_H__

#include <signal.h>

#include <boost/scoped_ptr.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/sync/named_condition.hpp>

#include "process_bus.h"

class LogServer
{
    public:
        LogServer();
        ~LogServer();

        int Init(const char* confpath);

        void Run();

    private:
        typedef boost::interprocess::named_mutex MutexType;
        typedef boost::interprocess::named_condition ConditionType;
        static void Stop(int);

        void InitSyncTools();
        void MonitorRoutine();
        void WorkerRoutine();

        boost::scoped_ptr<MutexType> mutex_;
        boost::scoped_ptr<ConditionType> cond_;
        boost::scoped_ptr<ProcessBus> process_bus_;

        std::string prefix_;
        unsigned worker_num_;
        static sig_atomic_t running_;
};

#endif   /* ----- #ifndef __LOGSVRD_H__  ----- */
