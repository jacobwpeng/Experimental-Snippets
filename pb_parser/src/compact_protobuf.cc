/*
 * =====================================================================================
 *
 *       Filename:  compact_protobuf.cc
 *        Created:  06/09/14 14:17:04
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "compact_protobuf.h"
#include "protobuf_parser.h"
#include "protobuf_helper.h"

#include <cstdio>
#include <boost/foreach.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/wire_format.h>

namespace CompactProtobuf
{

    /*-----------------------------------------------------------------------------
     *  Field
     *-----------------------------------------------------------------------------*/
    Value * Field::value()
    {
        assert (values.size() == 1);
        return &values[0];
    }
    /*-----------------------------------------------------------------------------
     *  Environment
     *-----------------------------------------------------------------------------*/
    Environment::Environment()
    {
    }

    Environment::~Environment()
    {
    }

    bool Environment::Register(const Slice& slice)
    {
        FileDescriptorSet protos;
        bool ok = protos.ParseFromArray(slice.start, slice.end - slice.start);
        if (ok)
        {
            for (int i = 0; i != protos.file_size(); ++i)
            {
                ok = pool_.BuildFile(protos.file(i));
                if (not ok) return false;
            }
            return true;
        }
        return false;
    }

    const Descriptor* Environment::FindMessageTypeByName(const string& name)
    {
        return pool_.FindMessageTypeByName(name);
    }

    Message::Message(const Descriptor* descriptor)
        :descriptor_(descriptor)
    {
    }

    Message::~Message()
    {
        for (size_t i = 0; i != embedded_messages_.size(); ++i)
        {
            delete embedded_messages_[i];
        }
    }

    bool Message::Init(const Slice& slice)
    {
        bool ok = Helper::ParseMessageLazy(slice, descriptor_, &fields_);
        if (not ok) return false;

        for (FieldMap::const_iterator iter = fields_.begin(); iter != fields_.end(); ++iter)
        {
            if (not iter->second.unknown) continue;

            if (not unknown_fields_) unknown_fields_.reset(new FieldMap);

            unknown_fields_->insert(*iter);
        }

        if (unknown_fields_)
        {
            for (FieldMap::const_iterator iter = unknown_fields_->begin(); iter != unknown_fields_->end(); ++iter)
            {
                fields_.erase(iter->first);
            }
        }
        return true;
    }

    bool Message::has_unknown_fields() const
    {
        return unknown_fields_ != NULL and unknown_fields_->size() != 0u;
    }

    void Message::Clear()
    {
        /* clear everything except unknwon fields in this message and all SubMessages */
    }

    uint32_t Message::GetInteger(const string& name, size_t idx, uint32_t * hi)
    {
        const FieldDescriptor* field_descriptor = descriptor_->FindFieldByName(name);
        assert (field_descriptor);
        FieldDescriptor::CppType cpp_type = field_descriptor->cpp_type();
        assert (cpp_type == FieldDescriptor::CPPTYPE_INT32 
                or cpp_type == FieldDescriptor::CPPTYPE_INT64
                or cpp_type == FieldDescriptor::CPPTYPE_UINT32
                or cpp_type == FieldDescriptor::CPPTYPE_UINT64
                or cpp_type == FieldDescriptor::CPPTYPE_BOOL
                or cpp_type == FieldDescriptor::CPPTYPE_ENUM);

        int field_id = field_descriptor->number();
        FieldMap::iterator iter = fields_.find(field_id);
        if (iter == fields_.end())              /* field no found */
        {
            if (field_descriptor->is_repeated()) assert (false); /* cannot index a empty repeated field with idx*/
            uint64_t default_value = Helper::DefaultIntegerValue(field_descriptor);
            return detail::DecodeUInt64(default_value, hi);
        }

        Field & field = iter->second;

        if (field_descriptor->is_repeated() and idx >= field.values.size())
        {
            /* out of range */
            assert (false);
        }
        else if (not field.decoded)      /* field not decoded */
        {
            if (not Helper::DecodeField(&field, field_descriptor)) assert (false); /* invalid data */
        }
        else {}

        assert (field.decoded);
        if (not field_descriptor->is_repeated()) idx = 0;
        uint64_t val = Helper::RetrieveIntegerValue(field, idx);
        return detail::DecodeUInt64(val, hi);
    }

    string Message::GetString(const string& name, size_t idx)
    {
        const FieldDescriptor* field_descriptor = descriptor_->FindFieldByName(name);
        assert (field_descriptor);
        FieldDescriptor::CppType cpp_type = field_descriptor->cpp_type();
        assert (cpp_type == FieldDescriptor::CPPTYPE_STRING);

        int field_id = field_descriptor->number();
        FieldMap::iterator iter = fields_.find(field_id);
        if (iter == fields_.end())
        {
            return field_descriptor->default_value_string();
        }

        Field & field = iter->second;
        if (field_descriptor->is_repeated() and idx >= field.values.size())
        {
            assert (false);                     /* out of range */
        }
        else if (not field.decoded)
        {
            if (not Helper::DecodeField(&field, field_descriptor) ) assert(false); /* invalid data */
        }
        else {}

        if (field_descriptor->is_repeated() == false) idx = 0;

        assert (field.decoded);
        return field.values[idx].decoded.s;
    }

    double Message::GetReal(const string& name, size_t idx)
    {
        const FieldDescriptor* field_descriptor = descriptor_->FindFieldByName(name);
        assert (field_descriptor);
        FieldDescriptor::CppType cpp_type = field_descriptor->cpp_type();
        assert (cpp_type == FieldDescriptor::CPPTYPE_DOUBLE
                or cpp_type == FieldDescriptor::CPPTYPE_FLOAT);

        int field_id = field_descriptor->number();
        FieldMap::iterator iter = fields_.find(field_id);
        if (iter == fields_.end())
        {
            return Helper::DefaultRealValue(field_descriptor);
        }

        Field & field = iter->second;
        if (field_descriptor->is_repeated() and idx >= field.values.size())
        {
            assert (false);                     /* out of range */
        }
        else if (not field.decoded)
        {
            Helper::DecodeField(&field, field_descriptor);
        }
        else {}

        assert (field.decoded);

        if (field_descriptor->is_repeated() == false) idx = 0;
        return Helper::RetrieveIntegerValue(field, idx);
    }

    Message * Message::GetMessage(const string& name, size_t idx)
    {
        const FieldDescriptor* field_descriptor = descriptor_->FindFieldByName(name);
        assert (field_descriptor);
        FieldDescriptor::CppType cpp_type = field_descriptor->cpp_type();
        assert (cpp_type == FieldDescriptor::CPPTYPE_MESSAGE);

        int field_id = field_descriptor->number();
        FieldMap::iterator iter = fields_.find(field_id);
        if (iter == fields_.end())
        {
            const Descriptor* descriptor = field_descriptor->message_type();
            assert (descriptor != NULL);

            Message * embedded_message = Helper::MakeMessage(field_descriptor);
            embedded_messages_.push_back(embedded_message);

            Field field;
            field.decoded = true;
            field.id = field_id;
            field.unknown = false;
            field.field_descriptor = field_descriptor;
            struct Value v;
            v.decoded.trivial.m = embedded_message;
            field.values.push_back(v);
            fields_.insert(std::make_pair(field_id, field));

            return embedded_message;
        }

        Field & field = iter->second;
        if (field_descriptor->is_repeated() and idx >= field.values.size())
        {
            assert (false);                     /* out of range */
        }
        else if (not field.decoded)
        {
            if (not Helper::DecodeField(&field, field_descriptor) ) assert(false); /* invalid data */
            BOOST_FOREACH(struct Value& value, field.values)
            {
                embedded_messages_.push_back(value.decoded.trivial.m);
            }
        }
        else {}

        assert (field.decoded);
        if (not field_descriptor->is_repeated()) idx = 0;
        return field.values[idx].decoded.trivial.m;
    }

    size_t Message::GetRepeatedSize(const string& name)
    {
        const FieldDescriptor* field_descriptor = descriptor_->FindFieldByName(name);
        assert (field_descriptor);

        int field_id = field_descriptor->number();
        FieldMap::iterator iter = fields_.find(field_id);
        if (iter == fields_.end()) return 0;
        else return iter->second.values.size();
    }

    bool Message::ToString(std::string* output)
    {
        BOOST_FOREACH(const FieldMap::value_type p, fields_)
        {
            const Field& field = p.second;
            /* generate tag */
            int tag = field.id;
            tag <<= 3;
            tag |= Helper::WireType(field.field_descriptor);
            /* output->Append(tag) */

            if (not field.decoded)
            {
            }
            else
            {

            }
        }
        return false;
    }

    namespace detail
    {
        uint32_t DecodeUInt64(uint64_t val, uint32_t * hi)
        {
            if (hi) *hi = (val >> 32) & 0xffffffff;
            return val & 0xffffffff;
        }
    }
}
