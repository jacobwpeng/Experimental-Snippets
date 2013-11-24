/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  11/20/2013 12:17:03 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  test new features
 *
 * =====================================================================================
 */

#include <signal.h>
#include <zmq.hpp>
#include <boost/format.hpp>

#define BOOST_ALL_DYN_LINK
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "processor_mgr.h"
#include "dlfcn.h"

using namespace std;
using boost::format;
using boost::str;
using boost::lexical_cast;
using namespace boost::log::trivial;

#define LOG BOOST_LOG_TRIVIAL

typedef IProcessor* (*Creator)(const char*);
typedef void (*Destroyer)(IProcessor*);

static const size_t MAX_CLIENT = 10;
static const size_t MAX_MSG_LEN = 100;
string g_libso_name;
void* g_handle;

Creator g_creator;
Destroyer g_destroyer;

void* LoadLibSO(const char* libso_name, Creator* pCreator, Destroyer* pDestroyer);

void sig_handle(int sig)
{
    if( sig != SIGUSR1 ) return;
    LOG(info) << "catch SIGUSR1!";
    if( NULL == LoadLibSO(g_libso_name.c_str(), &g_creator, &g_destroyer) )
    {
        LOG(error) << "LoadLibSO failed!";
    }
}

void* LoadLibSO(const char* libso_name, Creator* pCreator, Destroyer* pDestroyer)
{
    if( g_handle ) dlclose( g_handle );
    void* handle = dlopen( libso_name, RTLD_LAZY );
    if( handle == NULL )
    {
        LOG(error) << dlerror();
        return NULL;
    }

    Creator creator = (Creator)dlsym(handle, "CreateProcessor");
    if( creator == NULL )
    {
        LOG(error) << dlerror();
        return NULL;
    }
    *pCreator = creator;
    Destroyer destroyer = (Destroyer)dlsym(handle, "DestroyProcessor");
    if( destroyer == NULL )
    {
        LOG(error) << dlerror();
        return NULL;
    }
    *pDestroyer = destroyer;
    g_libso_name.assign( libso_name );
    g_handle = handle;
    LOG(info) << "load " << libso_name << " Done!";
    return handle;
}

string ProcessReq(const char* processor_name, int a, int b)
{
    IProcessor* p = g_creator(processor_name);
    if( p == NULL )
    {
        format err_fmt = format("create %s processor failed!");
        LOG(warning) << "create " << processor_name << " IProcessor failed!";
        return str( format("no %s processor is at service!") % processor_name);
    }
    format f = format("a = %d, b = %d, op = %s, result = %d") % a % b % processor_name % p->DoCalculate(a,b);
    LOG(info) << f;
    g_destroyer(p);
    return str(f);
}

int StartService(unsigned port)
{
    using namespace zmq;
    context_t ctx(1);
    vector<zmq_pollitem_t> clients;
    socket_t sock(ctx, ZMQ_REP);
    sock.bind( str(format("tcp://0.0.0.0:%u") % port).c_str() );

    zmq_pollitem_t listener;
    listener.socket = sock;
    listener.events = ZMQ_POLLIN;
    clients.push_back( listener );
    while(1)
    {
        try{
            int ret = poll( clients.data(), clients.size());
            LOG(info) << "ret = " << ret;
            if( ret < 0 )
            {
                LOG(error) << "poll returns " << ret;
                return ret;
            }
            if( clients[0].revents & ZMQ_POLLIN )
            {
                LOG(info) << "new connection";
                message_t request;
                sock.recv(&request);
                string req( static_cast<const char*>(request.data()), request.size() );
                LOG(info) << "receive from client : " << req;
                vector<string> results;
                boost::split(results, req, boost::is_any_of("|"));
                int a = lexical_cast<int>( results[1] );
                int b = lexical_cast<int>( results[2] );

                string rep = ProcessReq(results[0].c_str(), a, b);
                message_t reply(rep.length());
                memcpy(reply.data(), rep.data(), rep.size());

                sock.send(reply);
            }
        }catch(zmq::error_t& e)
        {
            if( e.num() == 4 )
                continue;
            else 
                LOG(error) << format("error while listening, errnum : %d, errmsg : %s") % e.num() % e.what();
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if( argc != 2 ) 
    { 
        LOG(error) << "Usage : " << argv[0] << " libso " << '\n';
        return -1; 
    }
    char cwd[1024];
    LOG(info) << getcwd(cwd, sizeof(cwd));
    pid_t pid = fork();
    if( pid != 0 )
    {
        /* parent, return immediately */
        LOG(info) << pid;
        return 0;
    }
    else
    {
        namespace logging = boost::log;
        logging::add_file_log("server.log");
        logging::core::get()->set_filter
        (
            logging::trivial::severity >= logging::trivial::info
        );
        string so_path = str(format("%s/%s") % cwd % argv[1]);
        void* handle = LoadLibSO( so_path.c_str() , &g_creator, &g_destroyer);
        if( handle == NULL )
        {
            return -2;
        }
        signal(SIGUSR1, sig_handle);
        int ret = StartService(9000);
        if( ret != 0 ) { return ret; }
        //ProcessReq("add", 5, 10);
        dlclose(g_handle);
        return 0;
    }
}
