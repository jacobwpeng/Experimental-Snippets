/*
 * =====================================================================================
 *
 *       Filename:  arena_server.cc
 *        Created:  08/22/14 16:24:07
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "arena_server.h"
#include "udp_server.h"
#include "event_loop.h"
#include <boost/bind.hpp>
#include <glog/logging.h>

using fx::net::UdpServer;
using fx::net::EventLoop;

ArenaServer::ArenaServer(const std::string& ip, int port)
    :ip_(ip), port_(port)
{
}

ArenaServer::~ArenaServer()
{
}

void ArenaServer::Start()
{
    EventLoop loop;
    server_.reset (new UdpServer(&loop, ip_, port_));
    server_->set_read_callback(boost::bind(&ArenaServer::OnRead, this, _1));
    server_->Start();
    loop.Run();
}

std::string ArenaServer::OnRead(const std::string& message)
{
    return message;
}
