/*
 * =====================================================================================
 *
 *       Filename:  protobuf_parser.cc
 *        Created:  06/09/14 15:02:01
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  protobuf parser
 *
 * =====================================================================================
 */

#include "protobuf_parser.h"

#include <cassert>

namespace CompactProtobuf
{
    namespace ProtobufParser
    {
        int GetFieldId(uint64_t val)
        {
            return val >> 3;
        }

        WireType GetWireType(uint64_t val)
        {
            return static_cast<WireType>(val & 0x7);
        }

        bool More(Byte byte)
        {
            return byte & 0x80;
        }

        uint64_t Value(Byte byte)
        {
            return byte & 0x7f;
        }

        ParserStatus ParseTag(ParserState* state)
        {
            ParserStatus status = ParseVarint(state);
            if (status != kOk) return status;
            uint64_t tag = state->v.decoded.trivial.varint;
            state->field_id = GetFieldId(tag);
            state->wire_type = GetWireType(tag);

            if (state->wire_type != 0 and state->wire_type != 1 and state->wire_type != 2 and state->wire_type != 5) return kInvalidWireType;
            return kOk;
        }

        ParserStatus ParseVarintInternal(ParserState* state, unsigned* plen)
        {
            Byte * cur = state->slice.start + state->pos;
            const unsigned kMaxVarintLength = 10u;        /* for maximum (u)int64 */
            unsigned len = 1;
            uint64_t val = 0;
            while (cur < state->slice.end and len < kMaxVarintLength)
            {
                uint64_t part = Value(*cur);
                part <<= ((len-1) * 7);
                val |= part;
                if (not More(*cur)) break;
                ++cur;
                ++len;
            }
            state->v.decoded.trivial.varint = val;
            *plen = len;
            return kOk;
        }

        ParserStatus ParseVarint(ParserState* state)
        {
            unsigned len = 0;
            ParserStatus status = ParseVarintInternal(state, &len);
            if (status != kOk) return status;
            state->pos += len;
            return kOk;
        }

        ParserStatus ParseVarintLazy(ParserState* state)
        {
            state->v.encoded.start = state->slice.start + state->pos;
            Byte * cur = state->v.encoded.start;
            const unsigned kMaxVarintLength = 10u;        /* for maximum (u)int64 */
            unsigned len = 1;
            while (cur < state->slice.end and len < kMaxVarintLength)
            {
                if (not More(*cur)) break;
                ++cur;
                ++len;
            }
            state->pos += len;
            state->v.encoded.end = state->slice.start + state->pos;
            return kOk;
        }

        ParserStatus ParseLengthDelimited(ParserState * state)
        {
            ParserStatus status = ParseVarint(state);
            if (status != kOk) return status;

            uint64_t len = state->v.decoded.trivial.varint;
            if (state->slice.start + state->pos + len > state->slice.end) return kNeedsMore;

            /*-----------------------------------------------------------------------------
             *  1 string, bytes
             *  2 embedded messages
             *  3 packed repeated
             *-----------------------------------------------------------------------------*/
            state->v.decoded.s.assign( reinterpret_cast<char*>(state->v.encoded.start) + state->pos, len );
            state->pos += len;
            return kOk;
        }

        ParserStatus ParseLengthDelimitedLazy(ParserState * state)
        {
            unsigned len = 0;
            ParserStatus status = ParseVarintInternal(state, &len);
            if (status != kOk) return status;

            uint64_t part_len = state->v.decoded.trivial.varint;
            if (state->slice.start + state->pos + len + part_len > state->slice.end) return kNeedsMore;

            state->v.encoded.start = state->slice.start + state->pos;
            state->v.encoded.end = state->slice.start + state->pos + len + part_len;
            state->pos += len + part_len;
            state->v.decoded.len = len;
            return kOk;
        }

        ParserStatus Parse64Bits(ParserState * state)
        {
            Byte * cur = state->slice.start + state->pos;
            if (cur + 8 > state->slice.end) return kNeedsMore;

            state->v.decoded.trivial.d.d = *((double*)(cur));
            state->pos += 8;
            return kOk;
        }

        ParserStatus Parse64BitsLazy(ParserState * state)
        {
            Byte * cur = state->slice.start + state->pos;
            if (cur + 8 > state->slice.end) return kNeedsMore;

            state->v.encoded.start = cur;
            state->v.encoded.end = cur + 8;
            state->pos += 8;
            return kOk;
        }

        ParserStatus Parse32Bits(ParserState * state)
        {
            Byte * cur = state->slice.start + state->pos;
            if (cur + 4 > state->slice.end) return kNeedsMore;

            state->v.decoded.trivial.f.f = *((float*)(cur));
            state->pos += 4;
            return kOk;
        }

        ParserStatus Parse32BitsLazy(ParserState * state)
        {
            Byte * cur = state->slice.start + state->pos;
            if (cur + 4 > state->slice.end) return kNeedsMore;

            state->v.encoded.start = cur;
            state->v.encoded.end = cur + 4;
            state->pos += 4;
            return kOk;
        }
    }
}
