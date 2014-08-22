/*
 * =====================================================================================
 *
 *       Filename:  udp_listener.cc
 *        Created:  08/22/14 15:47:38
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "udp_listener.h"
#include "channel.h"
#include "event_loop.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <glog/logging.h>
#include <boost/bind.hpp>

namespace fx
{
    namespace net
    {
        UdpListener::UdpListener(EventLoop * loop)
            :loop_(loop), fd_(-1)
        {
        }

        UdpListener::~UdpListener()
        {
            if (channel_)
            {
                channel_->Remove();
                ::close(fd_);
            }
        }

        void UdpListener::BindOrAbort(const std::string& ip, int port)
        {
            fd_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            PCHECK(fd_ >= 0) << "create socket failed";

            struct sockaddr_in sa;

            memset(&sa, 0x0, sizeof(sa));

            sa.sin_family = AF_INET;
            int ret = ::inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);
            PCHECK(ret == 1) << "inet_pton failed, ip[" << ip << "], port[" << port << "]";

            sa.sin_port = htons(port);
            ret = bind(fd_, reinterpret_cast<const struct sockaddr *>(&sa), sizeof(sa));
            PCHECK(ret == 0) << "bind failed";
        }

        void UdpListener::Start()
        {
            channel_.reset(new Channel(loop_, fd_));
            channel_->set_read_callback(boost::bind(&UdpListener::OnMessage, this));
            channel_->EnableReading();
            channel_->DisableWriting();
        }

        void UdpListener::OnMessage()
        {
            const size_t kMaxbufferLength = 1024; //1KiB
            char buf[kMaxbufferLength];
            struct sockaddr_in ca;
            socklen_t len = sizeof(ca);
            ssize_t bytes = ::recvfrom(fd_, buf, sizeof(buf), MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), &len);
            if (bytes < 0)
            {
                PLOG(WARNING) << "recvfrom failed";
                return;
            }
            std::string reply;
            if (mcb_)
            {
                std::string in(buf, bytes);
                reply = mcb_(in);
            }
            ::sendto(fd_, reply.data(), reply.size(), MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), sizeof(ca));
        }
    }
}
