/*
 * =====================================================================================
 *
 *       Filename:  ring_buffer.h
 *        Created:  06/04/14 09:29:26
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __RING_BUFFER_H__
#define  __RING_BUFFER_H__

#include <boost/noncopyable.hpp>

class RingBuffer
{
    public:
        struct OffsetData
        {
            ptrdiff_t front_offset;
            ptrdiff_t back_offset;
            int padding;
            size_t size;
        };
        struct RingBufferElement
        {
            RingBufferElement()
                :len(0), buf(NULL)
            {
            }

            int len;
            char * buf;
        };

    public:
        RingBuffer(char* start, char* end, void* mem, bool init_offsetdata = false);
        bool Push(const char* buf, int len);
        char* Pop(int* plen);

        int used() const;
        int length() const;
        void PrintSelf() const;

        char* front() const { return offsets_->front_offset + start_; }
        char* back() const { return offsets_->back_offset + start_; }
        int padding() const { return offsets_->padding; }
        size_t size() const { return offsets_->size; }

        void set_front(char* front) { offsets_->front_offset = front - start_; }
        void set_back(char* back) { offsets_->back_offset = back - start_; }
        void set_padding(int padding) { offsets_->padding = padding; }
        void set_size(size_t size) { offsets_->size = size; }

    private:
        bool CheckCanWrite(int len, bool* needs_padding) const;
        void Write(const char* buf, int len, bool needs_padding);
        char* ReadNext(int* plen);

    private:
        char * const start_;
        char * const end_;
        const int length_;
        volatile OffsetData * offsets_;
};

#endif   /* ----- #ifndef __RING_BUFFER_H__  ----- */
