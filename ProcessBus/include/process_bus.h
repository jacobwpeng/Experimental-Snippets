/*
 * =====================================================================================
 *
 *       Filename:  process_bus.h
 *        Created:  06/05/14 10:11:36
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  bus for IPC
 *
 * =====================================================================================
 */

#ifndef  __PROCESS_BUS_H__
#define  __PROCESS_BUS_H__

#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include "ring_buffer.h"

class ProcessBus : boost::noncopyable
{
    public:
        struct BusElement
        {
            int len;
            const char * buf;
        };

        ProcessBus();
        ~ProcessBus();

        int Init(const boost::property_tree::ptree& pt);
        int Connect();
        int Listen();

        bool Write(const BusElement& e);
        BusElement Read();

        size_t size() const;
    
    private:
        bool Inited() const;
        std::string GenerateMMapFilename() const;

    private:
        unsigned bus_id_;
        unsigned size_;
        void * mem_;
        size_t mmap_length_;
        int fd_;
        std::string filepath_;
        boost::scoped_ptr<RingBuffer> buffer_;
};

#endif   /* ----- #ifndef __PROCESS_BUS_H__  ----- */
