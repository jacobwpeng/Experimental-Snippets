/*
 * =====================================================================================
 *
 *       Filename:  fixed_size_buffer.hpp
 *        Created:  06/02/14 18:23:37
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __FIXED_SIZE_BUFFER_HPP__
#define  __FIXED_SIZE_BUFFER_HPP__

template<size_t size>
class FixedSizeBuffer : boost::noncopyable
{
    public:
        FixedSizeBuffer()
            :used_(0)
        {
        }

        size_t used() const { return used_; }
        size_t Available() const { return size - used_; }
        void ClearUsed() { used_ = 0u; }
        const char* buf() { return buf_; }
        void Append(const char* buf, size_t len)
        {
            assert (Available() >= len); 
            memcpy (buf_ + used_, buf, len);
            used_ += len;
        }

    private:
        char buf_[size];
        size_t used_;
};

#endif   /* ----- #ifndef __FIXED_SIZE_BUFFER_HPP__  ----- */
