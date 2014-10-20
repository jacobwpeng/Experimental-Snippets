/*
 * =====================================================================================
 *
 *       Filename:  event_loop.cc
 *        Created:  08/22/14 15:05:44
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "event_loop.h"

#include <boost/foreach.hpp>
#include <glog/logging.h>

#include "channel.h"
#include "poller.h"
#include "system_time.h"

namespace fx
{
    namespace net
    {
        EventLoop::EventLoop()
            :quit_(false), iteration_(0), wait_time_(20), idle_time_(100)
        {
            poller_.reset(new Poller);
        }

        EventLoop::~EventLoop()
        {
        }

        void EventLoop::Run()
        {
            fx::net::ChannelList channels;

            int timeout = wait_time_;//ms
            unsigned idle = 0;

            while (not quit_)
            {
                fx::base::time::TimeStamp now = poller_->Poll(timeout, &channels);
                ++iteration_;
                //DLOG(INFO) << "now[" << now << "], iteration[" << iteration_ << "], timeout[" << timeout << "]";

                for(auto channel : channels)
                {
                    channel->HandleEvents();
                }
                if (channels.size() == 0) ++idle;
                else idle = 0;
                channels.clear();

                if (period_functor_) period_functor_(iteration_);

                if (idle >= 100) timeout = idle_time_;
                else timeout = wait_time_;
            }
            LOG(INFO) << "EventLoop exiting...";
        }

        void EventLoop::Quit()
        {
            quit_ = true;
        }

        void EventLoop::UpdateChannel(Channel * channel)
        {
            poller_->UpdateChannel(channel);
        }

        void EventLoop::RemoveChannel(Channel * channel)
        {
            poller_->RemoveChannel(channel);
        }
    }
}
