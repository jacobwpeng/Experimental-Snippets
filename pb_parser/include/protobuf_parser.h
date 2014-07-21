/*
 * =====================================================================================
 *
 *       Filename:  protobuf_parser.h
 *        Created:  06/09/14 14:53:05
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  protobuf lazy parser
 *
 * =====================================================================================
 */

#ifndef  __PROTOBUF_PARSER_H__
#define  __PROTOBUF_PARSER_H__

#include "compact_protobuf.h"

namespace CompactProtobuf
{
    namespace ProtobufParser
    {
        enum ParserStatus
        {
            kOk = 1,
            kUnknownError = 2,
            kInvalidWireType = 3,
            kNeedsMore = 4,
        };

        struct ParserState
        {
            ParserState();
            static int obj_count;

            Slice slice;
            size_t pos;
            int field_id;
            WireType wire_type;
            Value v;
            const Descriptor* descriptor;
        };


        int GetFieldId(uint64_t val);      /* tag is a varint in range [0, (1<<29)-1] */
        WireType GetWireType(uint64_t val);
        bool More(Byte byte);
        uint64_t GetValue(Byte byte);              /* return uint64_t for bit operation */

        ParserStatus ParseTag(ParserState* state);
        ParserStatus ParseVarintInternal(ParserState* state, unsigned* plen);
        ParserStatus ParseVarint(ParserState* state);
        ParserStatus ParseVarintLazy(ParserState* state);
        ParserStatus ParseLengthDelimited(ParserState * state);
        ParserStatus ParseLengthDelimitedLazy(ParserState * state);
        ParserStatus Parse64Bits(ParserState * state);
        ParserStatus Parse64BitsLazy(ParserState * state);
        ParserStatus Parse32Bits(ParserState * state);
        ParserStatus Parse32BitsLazy(ParserState * state);
    }
}

#endif   /* ----- #ifndef __PROTOBUF_PARSER_H__  ----- */
