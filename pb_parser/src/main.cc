/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  06/08/14 11:09:12
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <stdint.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include "compact_protobuf.h"

//typedef uint8_t Byte;
//struct Slice
//{
//    Byte buf[1 << 20];
//    size_t len;
//};
//
//Slice slice;
//
//enum ParserStatus
//{
//    kOk = 1,
//    kUnknownError = 2,
//    kInvalidWireType = 3,
//};
//
//enum WireType
//{
//    kVarint = 0,
//    k64Bits = 1,
//    kLengthDelimited = 2,
//    k32Bits = 5
//};
//
//enum FieldType
//{
//    kTypeInt32 = 0,
//    kTypeInt64 = 1,
//    kTypeUInt32 = 2,
//    kTypeUInt64 = 3,
//    kTypeSInt32 = 4,
//    kTypeSInt64 = 5,
//    kTypeBool = 6,
//    kTypeEnum = 7,
//
//    kTypeFixed64 = 10,
//    kTypeSFixed64 = 11,
//    kTypeDouble = 12,
//
//    kTypeString = 20,
//    kTypeBytes = 21,
//    kTypeMessage = 22,
//    kTypePackedRepeated = 23,
//
//    kTypeFixed32 = 50,
//    kTypeSFixed32 = 51,
//    kTypeFloat = 52,
//};
//
//union Double64Bits
//{
//    uint64_t u;
//    double d;
//};
//
//union Float32Bits
//{
//    uint32_t u;
//    float f;
//};
//
//union Value
//{
//    uint64_t varint;
//    struct { Byte * start; Byte * end; } ld; /* length delimited */
//    Double64Bits d;
//    Float32Bits f;
//};
//
//struct ParserState
//{
//    Byte * start;                               /* buf start */
//    Byte * end;                                 /* buf end */
//    size_t pos;                                 /* current pos */
//    uint32_t field_id;
//    FieldType field_type;
//    WireType wire_type;
//    Value value;
//};
////std::vector<Value> values;
//
//const char * wire_type_to_string(WireType t)
//{
//    switch(t)
//    {
//        case kVarint:
//            return "varint";
//        case k64Bits:
//            return "64-bit";
//        case kLengthDelimited:
//            return "Length-delimited";
//        case k32Bits:
//            return "32-bit";
//        default:
//            return "unknown";
//    }
//}
//
//uint64_t get_field_id(uint64_t byte)
//{
//    return byte >> 3;
//}
//
//WireType get_wire_type(uint64_t byte)
//{
//    return static_cast<WireType>(byte & 0x7);
//}
//
//bool more(Byte byte)
//{
//    return byte & 0x80;
//}
//
//uint64_t value(Byte byte)
//{
//    return byte & 0x7f;
//}
//
//ParserStatus ParseTag(ParserState* state);
//ParserStatus ParseVarint(ParserState* state);
//ParserStatus ParseLengthDelimited(ParserState * state);
//ParserStatus Parse64Bits(ParserState * state);
//ParserStatus Parse32Bits(ParserState * state);
//
//ParserStatus ParseTag(ParserState* state)
//{
//    ParserStatus status = ParseVarint(state);
//    if (status != kOk) return status;
//    uint64_t tag = state->value.varint;
//    state->field_id = get_field_id(tag);
//    state->wire_type = get_wire_type(tag);
//
//    if (state->wire_type != 0 and state->wire_type != 1 and state->wire_type != 2 and state->wire_type != 5) return kInvalidWireType;
//    return kOk;
//}
//
//ParserStatus ParseVarint(ParserState* state)
//{
//    Byte * cur = state->start + state->pos;
//    const unsigned kMaxVarintLength = 10u;        /* for maximum (u)int64 */
//    unsigned len = 1;
//    uint64_t val = 0;
//    while (cur < state->end and len < kMaxVarintLength)
//    {
//        uint64_t part = value(*cur);
//        part <<= ((len-1) * 7);
//        val |= part;
//        if (not more(*cur)) break;
//        ++cur;
//        ++len;
//    }
//    state->pos += len;
//    state->value.varint = val;
//    return kOk;
//}
//
//ParserStatus ParseLengthDelimited(ParserState * state)
//{
//    ParserStatus status = ParseVarint(state);
//    if (status != kOk) return status;
//
//    uint64_t len = state->value.varint;
//    assert (state->start + state->pos + len <= state->end);
//    state->value.ld.start = state->start + state->pos;
//    state->value.ld.end = state->start + state->pos + len;
//    state->pos += len;
//    return kOk;
//}
//
//ParserStatus Parse64Bits(ParserState * state)
//{
//    state->value.d.u = *((uint64_t*)(state->start + state->pos));
//    state->pos += 8;
//    return kOk;
//}
//
//ParserStatus Parse32Bits(ParserState * state)
//{
//    state->value.f.u = *((uint32_t*)(state->start + state->pos));
//    state->pos += 4;
//    return kOk;
//}

bool ReadFile(const char* filename, CompactProtobuf::Slice* slice)
{
    using CompactProtobuf::Byte;
    FILE * fp = fopen(filename, "rb");
    if (fp == NULL) return false;

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    Byte* buf = new Byte[len];
    fseek(fp, 0, SEEK_SET);
    size_t bytes = fread(buf, sizeof(char), len, fp);
    slice->start = buf;
    slice->end = buf + len;
    return true;
}

int main(int argc, char* argv[])
{
    if (argc != 3) return -1;
    CompactProtobuf::Environment env;
    {
        CompactProtobuf::Slice slice;
        if (false == ReadFile(argv[1], &slice)) return -1;
        bool ok = env.Register(slice);
        printf("%s\n", ok ? "true" : "false");
        delete [] slice.start;
    }
    const CompactProtobuf::Descriptor * descriptor = env.FindMessageTypeByName("Pair");
    if (descriptor) printf("descriptor addr: %p\n", descriptor);

    CompactProtobuf::Slice slice;
    const char * filename = argv[2];
    
    if (ReadFile(filename, &slice) == false) return -2;
    printf("start: %p\tend: %p\n", slice.start, slice.end);

    CompactProtobuf::Message message(descriptor);
    bool ok = message.Init(slice);
    if (not ok) return -3;

    //{
    //    std::string s = message.GetString("s");
    //    printf("s: [%s]\n", s.c_str());
    //}
    //{
    //    std::string b = message.GetString("b");
    //    printf("b: [%s]\n", b.c_str());
    //}
    //{
    //    double d = message.GetReal("d", 0);
    //    printf("d: [%lf]\n", d);
    //}
    {
        CompactProtobuf::Message * t = message.GetMessage("t", 0);
        assert (t != NULL);
        printf("has_unknown_fields: %s\n", t->has_unknown_fields() ? "true" : "false");
    }
    //{
    //    float f = message.GetReal("f");
    //    printf("f: [%f]\n", f);
    //}
    //{
    //    uint32_t low = message.GetInteger("i32", NULL);
    //    printf("i32: %d\n", (int)low);
    //}
    //{
    //    uint32_t low = message.GetInteger("u32", NULL);
    //    printf("u32: %u\n", low);
    //}
    //{
    //    uint32_t low = message.GetInteger("sf32", NULL);
    //    printf("sf32: %u\n", low);
    //}
    //{
    //    uint32_t low = message.GetInteger("s32", NULL);
    //    printf("s32: %u\n", low);
    //}
    //{
    //    uint32_t low = 0, hi = 0;
    //    low = message.GetInteger("i64", &hi);
    //    uint64_t u64 = hi;
    //    u64 <<= 32;
    //    u64 |= low;

    //    printf("i64: %ld\n", (int64_t)u64);
    //}

    //{
    //    uint32_t low = 0, hi = 0;
    //    low = message.GetInteger("u64", &hi);
    //    uint64_t u64 = hi;
    //    u64 <<= 32;
    //    u64 |= low;
    //    printf("u64: %lu\n", u64);
    //}

    delete [] slice.start;
    return 0;
}
//
//    ParserState state;
//    state.start = slice.buf;
//    state.end = slice.buf + slice.len;
//    state.pos = 0;
//    ParserStatus status = kOk;
//    printf("start: %p, end: %p\n", state.start, state.end);
//    while (status == kOk and state.start + state.pos != state.end)
//    {
//        /* parse wire_type and field_id */
//        status = ParseTag(&state);
//        if (status != kOk) break;
//        printf("field_id: %u, wire_type: %s\n", state.field_id, wire_type_to_string(state.wire_type));
//
//        switch (state.wire_type)
//        {
//            case kVarint:
//                status = ParseVarint(&state);
//                if (status == kOk) printf("varint: %d\n", (int)state.value.varint);
//                break;
//            case kLengthDelimited:
//                status = ParseLengthDelimited(&state);
//                if (status == kOk) printf("start: %p, end: %p, length: %lu\n", state.value.ld.start, state.value.ld.end, state.value.ld.end - state.value.ld.start);
//                break;
//            case k64Bits:
//                status = Parse64Bits(&state);
//                if (status == kOk) printf("64-bit: %f\n", state.value.d.d);
//                break;
//            case k32Bits:
//                status = Parse32Bits(&state);
//                if (status == kOk) printf("32-bit: %f\n", state.value.f.f);
//                break;
//            default:
//                fprintf(stderr, "invalid state.wire_type: %u\n", state.wire_type);
//                status = kInvalidWireType;
//        }
//    }
//}
