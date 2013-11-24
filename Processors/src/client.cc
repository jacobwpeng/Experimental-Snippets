/*
 * =====================================================================================
 *
 *       Filename:  client.cc
 *        Created:  11/24/2013 11:16:24 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <boost/format.hpp>
#include <zmq.hpp>

#define BOOST_ALL_DYN_LINK
#include <boost/log/trivial.hpp>

using std::string;
using boost::format;
using boost::str;

#define LOG BOOST_LOG_TRIVIAL

static const unsigned MAX_REQUEST_TIMES = 1 << 20;
static const unsigned MAX_VAL = 1 << 15;
static const char* OP_TYPE_ARR[] = { "add", "minus", "mul", "div" };

int main()
{
    srand( time(NULL) );
    using namespace zmq;
    context_t ctx(1);
    socket_t sock(ctx, ZMQ_REQ);
    sock.connect("tcp://host700.pengwang.info:9000");

    unsigned times = 0;
    format fmt = format("%s|%d|%d");

    while( times != MAX_REQUEST_TIMES )
    {
        int a = rand() % MAX_VAL;
        int b = rand() % MAX_VAL;
        unsigned idx = rand() % 4;
        const char* op = OP_TYPE_ARR[idx];
        string req = str( fmt % OP_TYPE_ARR[idx] % a % b);
        LOG(info) << "send `" << req << "'";
        message_t request( req.length() );
        memcpy( request.data(), req.data(), req.length() );
        sock.send( request.data(), request.size() );
        ++times;

        message_t reply;
        sock.recv(&reply);
        LOG(info) << "got reply : " << string(static_cast<const char*>( reply.data() ), reply.size());
        usleep( 1000 );
    }
}


