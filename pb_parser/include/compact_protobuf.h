/*
 * =====================================================================================
 *
 *       Filename:  compact_protobuf.h
 *        Created:  06/09/14 10:52:41
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __COMPACT_PROTOBUF_H__
#define  __COMPACT_PROTOBUF_H__

#include <map>
#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>

#define WHERE fprintf(stderr, "[%s:%d]\n", __FILE__, __LINE__)

namespace CompactProtobuf
{
    typedef uint8_t Byte;
    using std::string;
    using std::map;
    using std::vector;

    using google::protobuf::Descriptor;
    using google::protobuf::DescriptorPool;
    using google::protobuf::FieldDescriptor;
    using google::protobuf::FileDescriptorSet;
    using google::protobuf::FileDescriptorProto;

    class Message;
    struct Slice
    {
        Byte * start;
        Byte * end;
    };
    union Double64Bits
    {
        uint64_t u;
        double d;
    };

    union Float32Bits
    {
        uint32_t u;
        float f;
    };

    struct Value
    {
        Slice encoded;
        struct 
        {
            union 
            { 
                uint64_t varint;
                Double64Bits d;
                Float32Bits f;
                Message * m;
            } trivial;
            string s;
            size_t len;
        } decoded;
    };

    struct Field
    {
        bool decoded;
        bool unknown;
        int id;
        const FieldDescriptor* field_descriptor;
        Value * value();
        vector<Value> values;
    };

    class Environment
    {
        public:
            Environment();
            ~Environment();
            bool Register(const Slice& slice);
            const Descriptor* FindMessageTypeByName(const string& name);

        private:
            DescriptorPool pool_;
    };

    typedef map<int, Field> FieldMap;

    class Message
    {
        public:
            Message(const Descriptor* descriptor);
            ~Message();

            bool Init(const Slice& slice);
            void Clear();
            bool has_unknown_fields() const;

            uint32_t GetInteger(const string& name, size_t idx, uint32_t * hi);
            string GetString(const string& name, size_t idx);
            double GetReal(const string& name, size_t idx);
            Message * GetMessage(const string& name, size_t idx);
            size_t GetRepeatedSize(const string& name);

            bool ToString(std::string* output);

        private:
            void AssertFieldDescriptor(const string& name, const FieldDescriptor* descriptor, FieldDescriptor::Type type);
            void DecodeField(Field* field);

        private:
            const Descriptor* descriptor_;
            FieldMap fields_;
            boost::scoped_ptr<FieldMap> unknown_fields_;
            vector<Message*> embedded_messages_;
    };

    namespace detail
    {
        uint32_t DecodeUInt64(uint64_t val, uint32_t * hi);
    }
};

#endif   /* ----- #ifndef __COMPACT_PROTOBUF_H__  ----- */
