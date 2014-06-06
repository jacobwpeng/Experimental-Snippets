/*
 * =====================================================================================
 *
 *       Filename:  process_bus.cc
 *        Created:  06/05/14 10:18:32
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  bus for IPC
 *
 * =====================================================================================
 */

#include "process_bus.h"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>

#include <sstream>
#include <boost/property_tree/ptree.hpp>

ProcessBus::ProcessBus()
    :bus_id_(0), size_(0), mem_(NULL), fd_(-1)
{
}

ProcessBus::~ProcessBus()
{
    if (mem_)
    {
        assert (mmap_length_ != 0);
        ::munmap(mem_, mmap_length_);
    }

    if (fd_)
    {
        close(fd_);
    }
}

int ProcessBus::Init(const boost::property_tree::ptree& pt)
{
    using namespace boost::property_tree;
    try
    {
        bus_id_ = pt.get<unsigned>("<xmlattr>.id");
        size_ = pt.get<unsigned>("<xmlattr>.size");
        filepath_ = pt.get<std::string>("<xmlattr>.path");
    }catch(ptree_error& e)
    {
        fprintf(stderr, "Init error, %s\n", e.what());
        return -1;
    }
    return 0;
}

int ProcessBus::Connect()
{
    assert (Inited());
    assert (mem_ == NULL);                      /* not connected or Listen */
    std::string mmap_filename = GenerateMMapFilename();
    fd_ = open(mmap_filename.c_str(), O_RDWR);
    if (fd_ < 0)
    {
        perror("open");
        return -1;
    }
    struct stat sb;
    if (fstat(fd_, &sb) < 0)
    {
        perror("fstat");
        return -2;
    }
    mmap_length_ = sb.st_size;

    mem_ = ::mmap(NULL, mmap_length_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -3;
    }

    char* start = static_cast<char*>(mem_) + sizeof(RingBuffer::OffsetData);
    char* end = static_cast<char*>(mem_) + mmap_length_;
    buffer_.reset (new RingBuffer(start, end, mem_));

    return 0;
}

int ProcessBus::Listen()
{
    assert (Inited());
    assert (mem_ == NULL);                      /* not connected or Listen */

    std::string mmap_filename = GenerateMMapFilename();
    fd_ = open(mmap_filename.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd_ < 0)
    {
        perror("open");
        return -1;
    }
    mmap_length_ = size_;
    ftruncate(fd_, mmap_length_);

    mem_ = ::mmap(NULL, mmap_length_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (mem_ == MAP_FAILED)
    {
        perror("mmap");
        return -2;
    }
    char* start = static_cast<char*>(mem_) + sizeof(RingBuffer::OffsetData);
    char* end = static_cast<char*>(mem_) + mmap_length_;
    buffer_.reset (new RingBuffer(start, end, mem_, true));

    return 0;
}

bool ProcessBus::Write(const ProcessBus::BusElement& e)
{
    assert (Inited());
    assert (mem_ != NULL);                      /* connect or listen */

    return buffer_->Push(e.buf, e.len);
}

ProcessBus::BusElement ProcessBus::Read()
{
    assert (Inited());
    assert (mem_ != NULL);                      /* connect or listen */
    ProcessBus::BusElement e;
    e.buf = buffer_->Pop(&e.len);
    return e;
}

size_t ProcessBus::size() const
{
    return buffer_->size();
}

bool ProcessBus::Inited() const
{
    return not (bus_id_ == 0 and filepath_.empty());
}

std::string ProcessBus::GenerateMMapFilename() const
{
    std::ostringstream oss;
    oss << filepath_ << '/' << bus_id_ << ".mmap";
    return oss.str();
}
