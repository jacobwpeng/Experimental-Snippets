/*
 * =====================================================================================
 *
 *       Filename:  protobuf_encoder.cc
 *        Created:  06/12/14 13:53:18
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  encoder of protobuf
 *
 * =====================================================================================
 */

#include "protobuf_encoder.h"
#include <cassert>
#include <cstdio>
#include <boost/foreach.hpp>

namespace CompactProtobuf
{
    namespace Encoder
    {
        EncoderBuffer::EncoderBuffer()
            :buf_(stack_buf_), pos_(0), capacity_(kStackBufLength)
        {
        }

        EncoderBuffer::~EncoderBuffer()
        {
            if (buf_ != stack_buf_)
            {
                /* heap buffer */
                free(buf_);
            }
        }

        void EncoderBuffer::Append(const EncoderBuffer& buf)
        {
            if (buf.size() + size() > capacity_)
            {
                ReallocBuffer();
            }

            assert (buf.size() + size() <= capacity_);
            memcpy(buf_ + pos_, buf.buf_, buf.size());
            pos_ += buf.size();
        }

        void EncoderBuffer::Append(const Byte* buf, size_t len)
        {
            if (len + size() > capacity_)
            {
                ReallocBuffer();
            }
            assert (len + size() <= capacity_);
            memcpy(buf_ + pos_, buf, len);
            pos_ += len;
        }

        void EncoderBuffer::Append(const Slice& slice)
        {
            assert (slice.end >= slice.start);
            size_t len = slice.end - slice.start;

            Append(slice.start, len);
        }

        void EncoderBuffer::ToString(std::string* output) const
        {
            assert (output);
            output->clear();
            output->assign(reinterpret_cast<char*>(buf_), size());
        }

        size_t EncoderBuffer::size() const
        {
            return pos_;
        }

        void EncoderBuffer::ReallocBuffer()
        {
            size_t new_capacity = capacity_ * 2;
            Byte * new_buf = static_cast<Byte*>( malloc(new_capacity * sizeof(Byte)) );
            memcpy(new_buf, buf_, size());

            if (buf_ != stack_buf_)
            {
                free(buf_);                 /* free heap buffer */
            }
            buf_ = new_buf;
            capacity_ = new_capacity;
        }

        uint64_t ZigZagEncode(const struct Value& value, FieldDescriptor::Type type)
        {
            uint64_t val = 0;
            uint32_t val_u32 = 0;
            switch (type)
            {
                case FieldDescriptor::TYPE_SINT32:
                    val_u32 = static_cast<uint32_t>(value.decoded.trivial.varint);
                    val = (val_u32 << 1) ^ (val_u32 >> 31);
                    break;

                case FieldDescriptor::TYPE_SINT64:
                    val = value.decoded.trivial.varint;
                    val = (val << 1) ^ (val >> 63);
                    break;
                default:
                    assert (false);
            }
            return val;
        }

        bool EncodeMessage(const Message* message, EncoderBuffer* buf)
        {
            assert (buf);
            /* check required fields */
            for (int idx = 0; idx != message->descriptor_->field_count(); ++idx)
            {
                const FieldDescriptor* field_descriptor = message->descriptor_->field(idx);
                if (field_descriptor->is_required() 
                        and message->fields_.find(field_descriptor->number()) == message->fields_.end())
                {
                    fprintf(stderr, "missing required field [%s]\n", field_descriptor->full_name().c_str());
                    return false;
                }
            }
            return EncodeMessage(message->fields_, message->unknown_fields_.get(), buf);
        }

        bool EncodeMessage(const FieldMap& fields, const FieldMap* unknown_fields, EncoderBuffer* buf)
        {
            assert (buf);
            BOOST_FOREACH(const FieldMap::value_type & p, fields)
            {
                const Field& field = p.second;
                uint32_t tag = field.id;
                tag <<= 3;
                tag |= static_cast<uint8_t>(field.wire_type);
                EncoderBuffer tag_buf;
                EncodeVarint(tag, &tag_buf);

                if (not field.decoded)
                {
                    BOOST_FOREACH(const struct Value& value, field.values)
                    {
                        buf->Append(tag_buf);
                        buf->Append(value.encoded);
                    }
                }
                else if (field.wire_type == kLengthDelimited)
                {
                    if (field.field_descriptor->is_packed())
                    {
                        /* repeated packed */
                        EncoderBuffer packed;
                        EncodePackedField(field, &packed);
                        EncoderBuffer len_buf;
                        EncodeVarint(packed.size(), &len_buf);
                        buf->Append(tag_buf);   /* tag */
                        buf->Append(len_buf);   /* length */
                        buf->Append(packed);    /* data */
                    }
                    else if (field.field_descriptor->type() == FieldDescriptor::TYPE_MESSAGE)
                    {
                        /* message */
                        BOOST_FOREACH(const struct Value& value, field.values)
                        {
                            const Message* message = value.decoded.m.get();
                            EncoderBuffer tmp;
                            if (false == EncodeMessage(message, &tmp)) return false;
                            EncoderBuffer len_buf;
                            EncodeVarint(tmp.size(), &len_buf);

                            buf->Append(tag_buf);
                            buf->Append(len_buf);
                            buf->Append(tmp);
                        }
                    }
                    else
                    {
                        /* string/bytes */
                        BOOST_FOREACH(const struct Value& value, field.values)
                        {
                            EncoderBuffer len_buf;
                            EncodeVarint(value.decoded.s.size(), &len_buf);
                            buf->Append(tag_buf);
                            buf->Append(len_buf);
                            buf->Append(reinterpret_cast<const Byte*>(value.decoded.s.data()), value.decoded.s.size());
                        }
                    }
                }
                else if (field.wire_type == kVarint)
                {
                    /* varint */
                    BOOST_FOREACH(const struct Value& value, field.values)
                    {
                        buf->Append(tag_buf);
                        EncodeVarint(value, field.field_descriptor->type(), buf);
                    }
                }
                else if (field.wire_type == k64Bits)
                {
                    /* fixed 64 bits */
                    BOOST_FOREACH(const struct Value& value, field.values)
                    {
                        buf->Append(tag_buf);
                        Encode64Bit(value, buf);
                    }
                }
                else if (field.wire_type == k32Bits)
                {
                    /* fixed 32 bits */
                    BOOST_FOREACH(const struct Value& value, field.values)
                    {
                        buf->Append(tag_buf);
                        Encode32Bit(value, buf);
                    }
                }
                else
                {
                    fprintf(stderr, "invalid wire_type: %d\n", field.wire_type);
                    assert (false);
                }
            }

            if (unknown_fields)
            {
                BOOST_FOREACH(const FieldMap::value_type & p, *unknown_fields)
                {
                    const Field& field = p.second;
                    uint32_t tag = field.id;
                    tag <<= 3;
                    tag |= static_cast<uint8_t>(field.wire_type);
                    EncoderBuffer tag_buf;
                    EncodeVarint(tag, &tag_buf);
                    
                    BOOST_FOREACH(const struct Value& value, field.values)
                    {
                        buf->Append(tag_buf);
                        buf->Append(value.encoded);
                    }
                }
            }
            return true;
        }

        void EncodeVarint(const struct Value& value, FieldDescriptor::Type type, EncoderBuffer* buf)
        {
            uint64_t val = 0;
            switch (type)
            {
                case FieldDescriptor::TYPE_SINT32:
                case FieldDescriptor::TYPE_SINT64:
                    val = ZigZagEncode(value, type);
                    EncodeVarint(val, buf);
                    break;
                case FieldDescriptor::TYPE_INT32:
                case FieldDescriptor::TYPE_INT64:
                case FieldDescriptor::TYPE_UINT32:
                case FieldDescriptor::TYPE_UINT64:
                case FieldDescriptor::TYPE_BOOL:
                case FieldDescriptor::TYPE_ENUM:
                    val = value.decoded.trivial.varint;
                    EncodeVarint(val, buf);
                    break;
                default:
                    assert (false);
            }
        }


        void EncodeVarint(uint64_t val, EncoderBuffer* buf)
        {
            assert (buf);
            Byte bytes[10] = {0};                     /* max varint length */
            unsigned len = 0;
            do
            {
                ++len;
                bytes[len-1] = val & 0x7f;
                val >>= 7;
                if (val != 0) bytes[len-1] |= 0x80; /* more */
            }while (len <= 10 and val != 0);

            buf->Append(bytes, len);
        }

        void Encode32Bit(const struct Value& value, EncoderBuffer* buf)
        {
            assert (buf);
            buf->Append(reinterpret_cast<const Byte*>(&value.decoded.trivial.f.u), 4);
        }

        void Encode64Bit(const struct Value& value, EncoderBuffer* buf)
        {
            assert (buf);
            buf->Append(reinterpret_cast<const Byte*>(&value.decoded.trivial.d.u), 8);
        }

        void EncodePackedField(const Field& field, EncoderBuffer* buf)
        {
            assert (field.field_descriptor->is_packed());
            assert (field.decoded);
            assert (buf);
            assert (buf->size() == 0);

            FieldDescriptor::Type type = field.field_descriptor->type();
            uint64_t val = 0;
            BOOST_FOREACH(const struct Value& value, field.values)
            {
                switch (type)
                {
                    case FieldDescriptor::TYPE_SINT32:
                    case FieldDescriptor::TYPE_SINT64:
                    case FieldDescriptor::TYPE_INT32:
                    case FieldDescriptor::TYPE_INT64:
                    case FieldDescriptor::TYPE_UINT32:
                    case FieldDescriptor::TYPE_UINT64:
                    case FieldDescriptor::TYPE_BOOL:
                    case FieldDescriptor::TYPE_ENUM:
                        EncodeVarint(value, type, buf);
                        break;
                    case FieldDescriptor::TYPE_FIXED32:
                    case FieldDescriptor::TYPE_SFIXED32:
                    case FieldDescriptor::TYPE_FLOAT:
                        Encode32Bit(value, buf);
                        break;
                    case FieldDescriptor::TYPE_FIXED64:
                    case FieldDescriptor::TYPE_SFIXED64:
                    case FieldDescriptor::TYPE_DOUBLE:
                        Encode64Bit(value, buf);
                        break;
                    default:
                        assert(false);
                }
            }
        }
    }
}
