/*
 * =====================================================================================
 *
 *       Filename:  poller.h
 *        Created:  08/22/14 14:40:03
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __POLLER_H__
#define  __POLLER_H__

#include <map>
#include <vector>
#include <boost/noncopyable.hpp>

#include "channel.h"
#include "system_time.h"

struct epoll_event;

namespace fx
{
    namespace net
    {
        class Poller : boost::noncopyable
        {
            public:
                Poller();
                ~Poller();

                fx::base::time::TimeStamp Poll(int timeout, ChannelList * active_channels);

                void UpdateChannel(Channel * channel);
                void RemoveChannel(Channel * channel);

            private:
                typedef std::vector<epoll_event> EventList;
                typedef std::map<int, Channel*> ChannelMap;

                void FillActiveChannels(int nevents, ChannelList * active_channels);

            private:
                const static int kMaxFdCount = 100;
                int epoll_fd_;
                ChannelMap channels_;
                EventList events_;
        };
    }
}

#endif   /* ----- #ifndef __POLLER_H__  ----- */
