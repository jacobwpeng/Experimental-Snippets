/*
 * =====================================================================================
 *
 *       Filename:  protobuf_encoder.h
 *        Created:  06/12/14 13:50:25
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  encoder of protobuf
 *
 * =====================================================================================
 */

#ifndef  __PROTOBUF_ENCODER_H__
#define  __PROTOBUF_ENCODER_H__

#include "compact_protobuf.h"
#include <boost/noncopyable.hpp>

namespace CompactProtobuf
{
    namespace Encoder
    {
        class EncoderBuffer : boost::noncopyable
        {
            public:
                EncoderBuffer();
                ~EncoderBuffer();

                void Append(const EncoderBuffer& buf);
                void Append(const Byte* buf, size_t len);
                void Append(const Slice& slice);
                size_t size() const;

                void ToString(std::string* output) const;
                uint8_t FirstByte() const { return *buf_; }

            private:
                void ReallocBuffer();

            private:
                static const size_t kStackBufLength = 1 << 12; /* 4K */

                Byte * buf_;
                size_t pos_;
                size_t capacity_;
                Byte stack_buf_[kStackBufLength];
        };

        uint64_t ZigZagEncode(const struct Value& value, FieldDescriptor::Type type);
        bool EncodeMessage(const Message* message, EncoderBuffer* buf);
        bool EncodeMessage(const FieldMap& fields, const FieldMap* unknown_fields, EncoderBuffer* buf);
        void EncodeVarint(uint64_t val, EncoderBuffer* buf);
        void EncodeVarint(const struct Value& value, FieldDescriptor::Type type, EncoderBuffer* buf);
        void EncodePackedField(const Field& field, EncoderBuffer* buf);
        void Encode32Bit(const struct Value& value, EncoderBuffer* buf);
        void Encode64Bit(const struct Value& value, EncoderBuffer* buf);
    }
}

#endif   /* ----- #ifndef __PROTOBUF_ENCODER_H__  ----- */
