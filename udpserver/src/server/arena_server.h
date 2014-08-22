/*
 * =====================================================================================
 *
 *       Filename:  arena_server.h
 *        Created:  08/22/14 16:22:50
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __ARENA_SERVER_H__
#define  __ARENA_SERVER_H__

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace fx { namespace net { class UdpServer; } }

class ArenaServer : boost::noncopyable
{
    public:
        ArenaServer(const std::string& ip, int port);
        ~ArenaServer();

        void Start();

    private:
        std::string OnRead(const std::string& message);

    private:
        boost::scoped_ptr<fx::net::UdpServer> server_;
        const std::string ip_;
        const int port_;
};

#endif   /* ----- #ifndef __ARENA_SERVER_H__  ----- */
