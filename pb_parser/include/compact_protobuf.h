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
#include <list>
#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor_database.h>

#define WHERE fprintf(stderr, "[%s:%d]\n", __FILE__, __LINE__)

namespace CompactProtobuf
{
    typedef uint8_t Byte;
    using std::string;
    using std::map;
    using std::list;
    using std::vector;

    using google::protobuf::Descriptor;
    using google::protobuf::DescriptorPool;
    using google::protobuf::FieldDescriptor;
    using google::protobuf::FileDescriptorSet;
    using google::protobuf::FileDescriptorProto;

    struct Field;
    class Message;
    typedef map<int, Field> FieldMap;
    typedef boost::shared_ptr<Message> MessagePtr;

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

    enum WireType
    {
        kVarint = 0,
        k64Bits = 1,
        kLengthDelimited = 2,
        k32Bits = 5
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
                size_t len;
            } primitive;
            MessagePtr m;
            string s;
        } decoded;
    };

    typedef vector<Value> ValueList;
    struct Field
    {
        bool decoded;
        bool unknown;
        int id;
        WireType wire_type;
        const FieldDescriptor* field_descriptor;
        ValueList values;

        bool has_value() const;
        Value * value();
        Value * value(size_t idx);
        const Value * value(size_t idx) const;
        Value Delete(size_t idx);
        void Append(const Value& value);
    };

    class Environment
    {
        public:
            Environment();
            ~Environment();
            bool Register(const Slice& slice);
            const Descriptor* FindMessageTypeByName(const string& name) const;

        private:
            DescriptorPool pool_;
    };

    namespace Encoder
    {
        class EncoderBuffer;
        bool EncodeMessage(const Message* message, EncoderBuffer* buf);
    };
    

    class Message
    {
        public:
            Message(const Descriptor* descriptor);
            ~Message();

            bool Init(const Slice& slice);
            bool FromString(const string& encoded);
            bool ToString(std::string* output);

            void Clear();
            bool has_field() const;
            bool has_field(const string& name) const;
            bool has_unknown_field() const;

            size_t GetFieldSize(const string& name);

            /*-----------------------------------------------------------------------------
             *  Field Get Operation
             *-----------------------------------------------------------------------------*/
            uint32_t GetInteger(const string& name, size_t idx, uint32_t * hi);
            double GetReal(const string& name, size_t idx);
            string GetString(const string& name, size_t idx);
            Message * GetMessage(const string& name, size_t idx);

            /*-----------------------------------------------------------------------------
             *  Field Set Operation
             *-----------------------------------------------------------------------------*/
            void SetInteger(const string& name, size_t idx, uint32_t low, uint32_t hi);
            void SetReal(const string& name, size_t idx, double val);
            void SetString(const string& name, size_t idx, const string& val);

            /*-----------------------------------------------------------------------------
             *  Field Add Operation
             *-----------------------------------------------------------------------------*/
            void AddInteger(const string& name, uint32_t low, uint32_t hi);
            void AddReal(const string& name, double val);
            void AddString(const string& name, const string& val);
            Message * AddMessage(const string& name);

            /*-----------------------------------------------------------------------------
             *  Field Delete Operation
             *-----------------------------------------------------------------------------*/
            uint32_t DeleteInteger(const string& name, size_t idx, uint32_t * hi);
            double DeleteReal(const string& name, size_t idx);
            string DeleteString(const string& name, size_t idx);
            MessagePtr DeleteMessage(const string& name, size_t idx);

        private:
            void CheckValidIndex(const Field& , const FieldDescriptor* , size_t idx);
            void TryDecodeField(Field* field, const FieldDescriptor* );
            const FieldDescriptor* CheckInteger(const string& name) const;
            const FieldDescriptor* CheckString(const string& name) const;
            const FieldDescriptor* CheckReal(const string& name) const;
            const FieldDescriptor* CheckMessage(const string& name) const;
            Message * InternalAddMessage(Field* field, const FieldDescriptor* field_descriptor);
            /*-----------------------------------------------------------------------------
             *  read-only access fields_ & unknown_fields_ & descriptor_
             *-----------------------------------------------------------------------------*/
            friend bool Encoder::EncodeMessage(const Message* message, Encoder::EncoderBuffer* buf);

        private:
            const Descriptor* descriptor_;
            FieldMap fields_;
            boost::scoped_ptr<FieldMap> unknown_fields_;
    };
};

#endif   /* ----- #ifndef __COMPACT_PROTOBUF_H__  ----- */
