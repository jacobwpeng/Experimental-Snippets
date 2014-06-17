/*
 * =====================================================================================
 *
 *       Filename:  protobuf_helper.h
 *        Created:  06/11/14 17:35:19
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  Helper utilities
 *
 * =====================================================================================
 */

#ifndef  __PROTOBUF_HELPER_H__
#define  __PROTOBUF_HELPER_H__

#include "compact_protobuf.h"

namespace CompactProtobuf
{
    namespace Helper
    {
        bool ParseMessageLazy(const Slice& slice, const Descriptor* descriptor, FieldMap* fields);
        bool CheckAllRequiredField(FieldMap* fileds, const Descriptor* descriptor);
        bool DecodeField(Field* field);
        bool DecodePacked(Field* field);
        bool DecodeVarint(Field* field);
        bool DecodeVarintWithZigZag(Field* field);
        bool DecodeFixed(Field* field);
        bool DecodeString(Field* field);
        bool DecodeMessage(Field* field);
        uint32_t DecodeUInt64(uint64_t val, uint32_t * hi);

        uint64_t DefaultIntegerValue(const FieldDescriptor* descriptor);
        uint64_t RetrieveIntegerValue(const Field& field, size_t idx);

        double DefaultRealValue(const FieldDescriptor* descriptor);
        double RetrieveRealValue(const Field& field, size_t idx);

        void SetInteger(struct Value* value, FieldDescriptor::Type type, uint64_t val);
        void SetReal(struct Value* value, FieldDescriptor::Type type, double val);
        void SetString(struct Value* value, FieldDescriptor::Type type, const string& val);

        MessagePtr MakeMessage(const FieldDescriptor* field_descriptor);
        WireType GetWireType(const FieldDescriptor* field_descriptor);

        void ZigzagDecode(uint64_t* val);
    }

    struct TraceTimeConsume
    {
        TraceTimeConsume(const char * file, int line);
        ~TraceTimeConsume();
        uint64_t s;
        uint64_t e;
        const char * file;
        int line;
    };
}

#define TRACE_TIME_CONSUME CompactProtobuf::TraceTimeConsume __tmp_time_consume__(__FILE__, __LINE__)

#endif   /* ----- #ifndef __PROTOBUF_HELPER_H__  ----- */
