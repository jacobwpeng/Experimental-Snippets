/*
 * =====================================================================================
 *
 *       Filename:  protobuf_helper.cc
 *        Created:  06/11/14 17:36:35
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "protobuf_helper.h"
#include "protobuf_parser.h"

#include <sys/time.h>
#include <cstdio>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>

uint64_t GetTimestamp()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == 0)
    {
        return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
    }
    return 0;
}

namespace CompactProtobuf
{
    TraceTimeConsume::TraceTimeConsume(const char* file, int line)
        :file(file), line(line)
    {
        s = GetTimestamp();
    }
    TraceTimeConsume::~TraceTimeConsume()
    {
        e = GetTimestamp();
        uint64_t interval = e - s;
        if (interval > 10) fprintf(stderr, "[%s:%d] %lu\n", file, line, (e-s));
    }

    namespace Helper
    {
        using namespace ProtobufParser;
        bool ParseMessageLazy(const Slice& slice, const Descriptor* descriptor, FieldMap* fields)
        {
            assert (descriptor);
            assert (fields);
            assert (slice.start <= slice.end);
            ParserState state;
            state.slice = slice;
            state.pos = 0;
            state.descriptor = descriptor;

            ParserStatus status = kOk;
            while (status == kOk and state.slice.start + state.pos < state.slice.end)
            {
                status = ParseTag(&state);
                if (status != kOk ) break;

                switch (state.wire_type)
                {
                    case kVarint:
                        status = ParseVarintLazy(&state);
                        break;
                    case kLengthDelimited:
                        status = ParseLengthDelimitedLazy(&state);
                        break;
                    case k64Bits:
                        status = Parse64BitsLazy(&state);
                        break;
                    case k32Bits:
                        status = Parse32BitsLazy(&state);
                        break;
                    default:
                        assert (false);
                }

                if (status != kOk) break;
                /* fill fields */
                FieldMap::iterator iter = fields->find(state.field_id);
                if (iter == fields->end())
                {
                    /* no entry for this field id */
                    Field field;
                    field.decoded = false;
                    field.id = state.field_id;

                    const FieldDescriptor* field_descriptor = descriptor->FindFieldByNumber(field.id);
                    field.unknown = (field_descriptor == NULL);
                    field.wire_type = state.wire_type;
                    field.values.push_back(state.v);
                    field.field_descriptor = field_descriptor;
                    fields->insert(std::make_pair(field.id, field));
                }
                else
                {
                    Field & field = iter->second;
                    /* we already got a entry for this field */
                    if (field.unknown or field.field_descriptor->label() == FieldDescriptor::LABEL_REPEATED)
                    {
                        /* unknown field or repeated field, both we save all entry*/
                        field.values.push_back(state.v);
                    }
                    else
                    {
                        /* for optional or required field, we only save the new one */
                        assert (field.values.size() == 1);
                        *field.value(0) = state.v; /* replace with the new one */
                    }
                }
            }
            if (status == kOk)
            {
                assert (CheckAllRequiredField(fields, descriptor));
            }
            else
            {
                fprintf(stderr, "ParseMessageLazy failed, status: %d\n", status);
            }
            return status == kOk;
        }

        bool CheckAllRequiredField(FieldMap* fields, const Descriptor* descriptor)
        {
            assert (fields);
            assert (descriptor);
            for (int i = 0; i != descriptor->field_count(); ++i)
            {
                const FieldDescriptor* field_descriptor = descriptor->field(i);
                if (field_descriptor->label() == FieldDescriptor::LABEL_REQUIRED)
                {
                    if (fields->find(field_descriptor->number()) == fields->end()) 
                    {
                        fprintf(stderr, "required field[%d] not found!\n", field_descriptor->number());
                        return false;
                    }
                }
            }
            return true;
        }

        bool DecodeField(Field* field, const FieldDescriptor* descriptor)
        {
            assert (field);
            assert (descriptor);

            switch(field->field_descriptor->type())
            {
                case FieldDescriptor::TYPE_INT32:
                case FieldDescriptor::TYPE_INT64:
                case FieldDescriptor::TYPE_UINT32:
                case FieldDescriptor::TYPE_UINT64:
                case FieldDescriptor::TYPE_BOOL:
                case FieldDescriptor::TYPE_ENUM:
                    return DecodeVarint(field, descriptor);

                case FieldDescriptor::TYPE_SINT32:
                case FieldDescriptor::TYPE_SINT64:
                    return DecodeVarintWithZigZag(field, descriptor);

                case FieldDescriptor::TYPE_FIXED32:
                case FieldDescriptor::TYPE_FIXED64:
                case FieldDescriptor::TYPE_SFIXED32:
                case FieldDescriptor::TYPE_SFIXED64:
                case FieldDescriptor::TYPE_DOUBLE:
                case FieldDescriptor::TYPE_FLOAT:
                    return DecodeFixed(field, descriptor);

                case FieldDescriptor::TYPE_STRING:
                case FieldDescriptor::TYPE_BYTES:
                    return DecodeString(field, descriptor);

                case FieldDescriptor::TYPE_MESSAGE:
                    return DecodeMessage(field, descriptor);

                case FieldDescriptor::TYPE_GROUP:
                    /* deprecated */
                    return false;
            }
        }

        bool DecodeVarint(Field* field, const FieldDescriptor* descriptor)
        {
            ProtobufParser::ParserState state;
            BOOST_FOREACH(struct Value& value, field->values)
            {
                state.slice.start = value.encoded.start;
                state.slice.end = value.encoded.end;
                state.pos = 0;
                ProtobufParser::ParserStatus status = ParseVarint(&state);
                if (status != ProtobufParser::kOk) return false;
                value.decoded.trivial.varint = state.v.decoded.trivial.varint;
            }
            field->decoded = true;
            return true;
        }

        bool DecodeVarintWithZigZag(Field* field, const FieldDescriptor* descriptor)
        {
            if (false == DecodeVarint(field, descriptor)) return false;
            assert (field->decoded);

            BOOST_FOREACH(struct Value& value, field->values)
            {
                if (field->field_descriptor->type() == FieldDescriptor::TYPE_SINT32 
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_SFIXED32)
                {
                    uint32_t val = static_cast<uint32_t>(value.decoded.trivial.varint);
                    value.decoded.trivial.varint = (val & 0x1) == 0 ? (val >> 1) : -(val >> 1) - 1;
                }
                else if (field->field_descriptor->type() == FieldDescriptor::TYPE_SINT64
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_SFIXED64)
                {
                    uint64_t val = value.decoded.trivial.varint;
                    value.decoded.trivial.varint = (val & 0x1) == 0 ? (val >> 1) : -(val >> 1) - 1;
                }
                else 
                { 
                    field->decoded = false;     /* revert DecodeVarint */
                    return false; 
                }
            }
            return true;
        }

        bool DecodeFixed(Field* field, const FieldDescriptor* descriptor)
        {
            BOOST_FOREACH(struct Value& value, field->values)
            {
                if (field->field_descriptor->type() == FieldDescriptor::TYPE_FIXED64 
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_SFIXED64
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_DOUBLE)
                {
                    value.decoded.trivial.d.d = *(double*)(value.encoded.start);
                }
                else if (field->field_descriptor->type() == FieldDescriptor::TYPE_FIXED32 
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_SFIXED32
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_FLOAT)
                {
                    value.decoded.trivial.f.f = *(float*)(value.encoded.start);
                }
                else { return false; }
            }
            field->decoded = true;
            return true;
        }

        bool DecodeString(Field* field, const FieldDescriptor* descriptor)
        {
            ParserState state;
            BOOST_FOREACH(struct Value& value, field->values)
            {
                if (field->field_descriptor->type() == FieldDescriptor::TYPE_STRING
                        or field->field_descriptor->type() == FieldDescriptor::TYPE_BYTES)
                {
                    state.slice.start = value.encoded.start;
                    state.slice.end = value.encoded.end;
                    state.pos = 0;
                    unsigned varint_part_length = 0;
                    ProtobufParser::ParserStatus status = ParseVarintInternal(&state, &varint_part_length);
                    if (status != ProtobufParser::kOk) return false;

                    size_t len = value.encoded.end - value.encoded.start - varint_part_length;
                    value.decoded.s.assign(reinterpret_cast<char*>(value.encoded.start + varint_part_length), len);
                }
                else { return false; }
            }
            field->decoded = true;
            return true;
        }

        bool DecodeMessage(Field* field, const FieldDescriptor* descriptor)
        {
            assert (field);
            assert (descriptor);
            assert (field->field_descriptor->type() == FieldDescriptor::TYPE_MESSAGE);

            BOOST_FOREACH(struct Value& value, field->values)
            {
                MessagePtr message = MakeMessage(descriptor);
                Slice slice;
                slice.start = value.encoded.start + value.decoded.len;
                slice.end = value.encoded.end;
                if (false == message->Init(slice) ) return false;

                value.decoded.m = message;
            }
            field->decoded = true;
            return true;
        }

        uint32_t DecodeUInt64(uint64_t val, uint32_t * hi)
        {
            if (hi) *hi = (val >> 32) & 0xffffffff;
            return val & 0xffffffff;
        }

        uint64_t DefaultIntegerValue(const FieldDescriptor* descriptor)
        {
            const google::protobuf::EnumValueDescriptor* enum_descriptor = NULL;
            switch (descriptor->cpp_type())
            {
                case FieldDescriptor::CPPTYPE_INT32:
                    return descriptor->default_value_int32();
                case FieldDescriptor::CPPTYPE_INT64:
                    return descriptor->default_value_int64();
                case FieldDescriptor::CPPTYPE_UINT32:
                    return descriptor->default_value_uint32();
                case FieldDescriptor::CPPTYPE_UINT64:
                    return descriptor->default_value_uint64();
                case FieldDescriptor::CPPTYPE_BOOL:
                    return descriptor->default_value_bool();
                case FieldDescriptor::CPPTYPE_ENUM:
                     enum_descriptor = descriptor->default_value_enum();
                    return enum_descriptor->number();
                default:
                    assert (false);
                    return 0;
            }
        }

        uint64_t RetrieveIntegerValue(const Field& field, size_t idx)
        {
            assert (idx < field.values.size());
            switch (field.field_descriptor->type())
            {
                case FieldDescriptor::TYPE_SINT32:
                case FieldDescriptor::TYPE_INT32:
                case FieldDescriptor::TYPE_UINT32:
                case FieldDescriptor::TYPE_BOOL:
                case FieldDescriptor::TYPE_ENUM:
                case FieldDescriptor::TYPE_INT64:
                case FieldDescriptor::TYPE_UINT64:
                case FieldDescriptor::TYPE_SINT64:
                    return field.value(idx)->decoded.trivial.varint;

                case FieldDescriptor::TYPE_SFIXED32:
                case FieldDescriptor::TYPE_FIXED32:
                    return field.value(idx)->decoded.trivial.f.u;

                case FieldDescriptor::TYPE_FIXED64:
                case FieldDescriptor::TYPE_SFIXED64:
                    return field.value(idx)->decoded.trivial.d.u;

                default:
                    assert (false);
                    return 0;
            }
        }

        void SetInteger(struct Value* value, FieldDescriptor::Type type, uint64_t val)
        {
            switch (type)
            {
                case FieldDescriptor::TYPE_SINT32:
                case FieldDescriptor::TYPE_INT32:
                case FieldDescriptor::TYPE_UINT32:
                case FieldDescriptor::TYPE_BOOL:
                case FieldDescriptor::TYPE_ENUM:
                case FieldDescriptor::TYPE_INT64:
                case FieldDescriptor::TYPE_UINT64:
                case FieldDescriptor::TYPE_SINT64:
                    value->decoded.trivial.varint = val;
                    break;
                case FieldDescriptor::TYPE_SFIXED32:
                case FieldDescriptor::TYPE_FIXED32:
                    value->decoded.trivial.f.u = val;
                    break;
                case FieldDescriptor::TYPE_FIXED64:
                case FieldDescriptor::TYPE_SFIXED64:
                    value->decoded.trivial.d.u = val;
                default:
                    assert(false);
            }
        }

        void SetReal(struct Value* value, FieldDescriptor::Type type, double val)
        {
            switch (type)
            {
                case FieldDescriptor::TYPE_FLOAT:
                    value->decoded.trivial.f.f = val;
                    break;
                case FieldDescriptor::TYPE_DOUBLE:
                    value->decoded.trivial.d.u = val;
                    break;
                default:
                    assert (false);
            }
        }

        void SetString(struct Value* value, FieldDescriptor::Type type, const string& val)
        {
            assert (type == FieldDescriptor::TYPE_BYTES 
                    or type == FieldDescriptor::TYPE_STRING);

            value->decoded.s = val;
        }

        double DefaultRealValue(const FieldDescriptor* descriptor)
        {
            switch (descriptor->cpp_type())
            {
                case FieldDescriptor::CPPTYPE_DOUBLE:
                    return descriptor->default_value_double();
                case FieldDescriptor::CPPTYPE_FLOAT:
                    return descriptor->default_value_float();
                default:
                    assert (false);
                    return 0;
            }
        }

        double RetrieveRealValue(const Field& field, size_t idx)
        {
            assert (idx < field.values.size());
            switch (field.field_descriptor->type())
            {
                case FieldDescriptor::TYPE_DOUBLE:
                    return field.value(idx)->decoded.trivial.d.d;
                case FieldDescriptor::TYPE_FLOAT:
                    return field.value(idx)->decoded.trivial.f.f;
                default:
                    assert (false);
                    return 0;
            }
        }

        MessagePtr MakeMessage(const FieldDescriptor* field_descriptor)
        {
            assert (field_descriptor->type() == FieldDescriptor::TYPE_MESSAGE);
            return boost::make_shared<Message>(field_descriptor->message_type());
        }

        WireType GetWireType(const FieldDescriptor* field_descriptor)
        {
            if (field_descriptor->is_packed()) return kLengthDelimited; /* length-delimited */
            switch (field_descriptor->type())
            {
                case FieldDescriptor::TYPE_INT32:
                case FieldDescriptor::TYPE_INT64:
                case FieldDescriptor::TYPE_UINT32:
                case FieldDescriptor::TYPE_UINT64:
                case FieldDescriptor::TYPE_BOOL:
                case FieldDescriptor::TYPE_ENUM:
                case FieldDescriptor::TYPE_SINT32:
                case FieldDescriptor::TYPE_SINT64:
                    return kVarint;                   /* varint */

                case FieldDescriptor::TYPE_FIXED32:
                case FieldDescriptor::TYPE_SFIXED32:
                case FieldDescriptor::TYPE_FLOAT:
                    return k32Bits;                   /* 32-bit */

                case FieldDescriptor::TYPE_FIXED64:
                case FieldDescriptor::TYPE_SFIXED64:
                case FieldDescriptor::TYPE_DOUBLE:
                    return k64Bits;                   /* 64-bit */

                case FieldDescriptor::TYPE_STRING:
                case FieldDescriptor::TYPE_BYTES:
                case FieldDescriptor::TYPE_MESSAGE:
                    return kLengthDelimited;                   /* length-delimited */

                case FieldDescriptor::TYPE_GROUP:
                    assert (false);
                    return kVarint;
            }
        }
    }
}
