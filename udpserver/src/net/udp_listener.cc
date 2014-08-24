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
            const size_t kMaxUdpMessageSize = 1 << 16; //64KiB
            char in[kMaxUdpMessageSize];
            char out[kMaxUdpMessageSize];
            struct sockaddr_in ca;
            socklen_t len = sizeof(ca);
            ssize_t bytes = ::recvfrom(fd_, in, sizeof(in), MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), &len);
            if (bytes < 0)
            {
                PLOG(WARNING) << "recvfrom failed";
                return;
            }
            if (mcb_)
            {
                ssize_t out_bytes = mcb_(in, bytes, out);
                if (out_bytes < 0)
                {
                    LOG(WARNING) << "MessageCallback return " << out_bytes;
                }
                else if (static_cast<size_t>(out_bytes) > kMaxUdpMessageSize)
                {
                    LOG(WARNING) << "bytes overflow, truncate output";
                    ::sendto(fd_, out, kMaxUdpMessageSize, MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), sizeof(ca));
                }
                else
                {
                    ::sendto(fd_, out, out_bytes, MSG_DONTWAIT, reinterpret_cast<struct sockaddr *>(&ca), sizeof(ca));
                }
            }
        }
    }
}
