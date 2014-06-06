/*
 * =====================================================================================
 *
 *       Filename:  ring_buffer.cc
 *        Created:  06/04/14 09:31:47
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  ring buffer
 *
 * =====================================================================================
 */

#include <cassert>
#include <cstdio>
#include <cstring>
#include "ring_buffer.h"
#include <map>

RingBuffer::RingBuffer(char * start, char * end, void* mem, bool init_offsetdata)
    :start_(start), end_(end), length_(end - start), offsets_(new (mem) OffsetData)//padding_(0), front_(start), back_(start), offset_(0)
{
    assert (end_ > start_);                    /* valid buffer range */
    assert (end_ - start_ > 1024);                /* min buffer range required */
    if (init_offsetdata)
    {
        set_padding(0);
        set_size(0);
        set_front(start);
        set_back(start);
    }
}

bool RingBuffer::Push(const char* buf, int len)
{
    assert (len >= 0);
    bool needs_padding;
    if (len == 0 or len >= length() or not CheckCanWrite(len, &needs_padding) ) return false;
    assert (buf != NULL);

    this->Write(buf, len, needs_padding);
    set_size( 1+size() );
    return true;
}

char* RingBuffer::Pop(int* plen)
{
    assert (plen);
    if (used() == 0)
    {
        *plen = 0;
        return NULL;
    }
    else
    {
        set_size( size()-1 );
        return ReadNext(plen);
    }
}

int RingBuffer::used() const
{
    char * front = this->front();
    char * back = this->back();
    if (back >= front) return back - front;
    else return back + length_ - front - padding();
}

int RingBuffer::length() const
{
    return length_;
}

void RingBuffer::PrintSelf() const
{
    printf("start: %p, end: %p, front_: %p, back_: %p, padding_: %d\n",
            start_, end_, this->front(), this->back(), padding());
}

bool RingBuffer::CheckCanWrite(int len, bool* needs_padding) const
{
    char * front = this->front();
    char * back = this->back();
    int total = sizeof len + len;
    *needs_padding = false;
    if (back >= front)
    {
        if (back+ total >= end_)
        {
            if (start_ + total >= front) return false;
            *needs_padding = true;
        }
    }
    else
    {
        if (back + total >= front) return false;
    }
    return true;
}

void RingBuffer::Write(const char* buf, int len, bool needs_padding)
{
    char * front = this->front();
    char * back = this->back();
    if (needs_padding)
    {
        /* skip all from front_ to end_ */
        assert (back >= front);
        (void)front;
        int padding = end_ - back;
        memset(back, 0x0, padding);
        set_padding(padding);
        set_back(start_);
    }

    back = this->back();

    memcpy(back, &len, sizeof len);
    back += sizeof len;
    memcpy(back, buf, len);
    back += len;
    set_back(back);
}

char* RingBuffer::ReadNext(int* plen)
{
    assert (*plen);
    char * front = this->front();
    /* adjust front_ ptr */
    if (front + padding() == end_)
    {
        set_padding(0);
        front = start_;
    }
    int len = *(int*)front;
    front += sizeof(len);

    *plen = len;
    char * buf = front;

    front += len;
    set_front(front);
    return buf;
}
