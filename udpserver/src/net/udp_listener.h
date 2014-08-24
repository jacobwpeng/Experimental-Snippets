/*
 * =====================================================================================
 *
 *       Filename:  udp_listener.h
 *        Created:  08/22/14 15:44:10
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  listen to udp message
 *
 * =====================================================================================
 */

#ifndef  __UDP_LISTENER_H__
#define  __UDP_LISTENER_H__

#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace fx
{
    namespace net
    {
        class Channel;
        class EventLoop;
        class UdpListener : boost::noncopyable
        {
            public:
                typedef boost::function< ssize_t(const char*, size_t, char*) > MessageCallback;

            public:
                UdpListener(EventLoop * loop);
                ~UdpListener();

                void set_message_callback(const MessageCallback& mcb) { mcb_ = mcb; }
                void BindOrAbort(const std::string& ip, int port);
                void Start();

            private:
                void OnMessage();

            private:
                EventLoop * loop_;
                int fd_;
                boost::scoped_ptr<Channel> channel_;
                MessageCallback mcb_;
        };
    }
}
#endif   /* ----- #ifndef __UDP_LISTENER_H__  ----- */
