/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  05/27/14 20:05:43
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
#include <zmq.h>

using namespace std;
using boost::thread;

static const unsigned kThreadCount = 4;

void ThreadRoutine(void * ctx)
{
    void * receiver = zmq_socket(ctx, ZMQ_REQ);
    int ret = zmq_connect( receiver, "inproc://workers" );
    assert (ret == 0);
    (void)ret;

    while (1)
    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        ret = zmq_msg_recv(&msg, receiver, ZMQ_DONTWAIT);
        if (ret < 0)
        {
            perror("zmq_recvmsg");
            exit(1);
        }

        int size = zmq_msg_size(&msg);
        char * data = static_cast<char*>(zmq_msg_data(&msg));
        string message(data, size);
        zmq_msg_close(&msg);
        cout << "msg: " << message << '\n';
    }
}

int main()
{
    void * ctx = zmq_ctx_new();
    void * clients = zmq_socket(ctx, ZMQ_ROUTER);
    zmq_bind(clients, "ipc:///tmp/zmq/clients");
    assert(clients);

    void * workers = zmq_socket(ctx, ZMQ_DEALER);
    zmq_bind(workers, "inproc://workers");
    assert(workers);
    
    boost::thread_group threads;
    for (unsigned i = 0; i != kThreadCount; ++i)
    {
        threads.create_thread( boost::bind(ThreadRoutine, ctx) );
    }

    zmq_proxy(clients, workers, NULL);

    threads.join_all();

    zmq_close(clients);
    zmq_close(workers);
    zmq_ctx_destroy(ctx);

    return 0;
}

//static const unsigned kLoopTimes = 1 << 21;
//static const char * msg = "Wikipedia's departure from the expert-driven style of encyclopedia-building and "
//                    "the presence of much unacademic content have received extensive attention in print media\n";
//
//void ThreadRoutine(FILE * file)
//{
//    unsigned len = strlen(msg);
//    for (int i = 0; i != kLoopTimes; ++i)
//    {
//        fwrite(msg, 1, len, file);
//    }
//}
//
//int main(int argc, char* argv[])
//{
//    if (argc != 2) return -1;
//
//    boost::thread_group threads;
//    FILE * file = fopen(argv[1], "a");
//    if (file == NULL)
//    {
//        perror("fopen");
//        return -2;
//    }
//
//
//    threads.join_all();
//    fclose(file);
//    return 0;
//}
