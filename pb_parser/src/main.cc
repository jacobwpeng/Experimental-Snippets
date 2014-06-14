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
#include <boost/bind.hpp>

#include "compact_protobuf.h"
#include "protobuf_helper.h"

#include "benchmark.h"
#include "userinfo.pb.h"

extern "C"
{
#include "pbc.h"
}

using namespace std;
using namespace petlib;

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

void WriteFile(const char* filename, const std::string& output)
{
    FILE * fp = fopen(filename, "wb");
    if (fp == NULL) return;

    fwrite(output.data(), 1, output.size(), fp);
    fclose(fp);
    return ;
}

const char * BoolToString(bool b)
{
    return b ? "true" : "false";
}

void TestStaticProtobuf(benchmark::BenchmarkState& state, const string& encoded)
{
    for (int x = 0; x != state.max_x; ++x)
    {
        HFPBUserInfo userinfo;
        if (userinfo.ParseFromString(encoded))
        {
            for (int i = 0; i != userinfo.goods_size(); ++i)
            {
                const HFPBUserGoods& goods = userinfo.goods(i);
                uint32_t goods_id = goods.goods_id();
                uint32_t goods_num = goods.goods_num();
            }
        }
        //string output;
        //userinfo.SerializeToString(&output);
    }
}

void TestDynamicProtobuf(benchmark::BenchmarkState& state, const CompactProtobuf::Environment& env, const CompactProtobuf::Slice& encoded)
{
    using CompactProtobuf::Message;
    for (int x = 0; x != state.max_x; ++x)
    {
        const CompactProtobuf::Descriptor * descriptor = env.FindMessageTypeByName("petlib.HFPBUserInfo");
        CompactProtobuf::Message message(descriptor);
        message.Init(encoded);

        int size = message.GetFieldSize("goods");
        for (int i = 0; i != size; ++i)
        {
            CompactProtobuf::Message * goods_msg = message.GetMessage("goods", i);
            uint32_t goods_id = goods_msg->GetInteger("goods_id", 0, NULL);
            uint32_t goods_num = goods_msg->GetInteger("goods_num", 0, NULL);
        }

        //string output;
        //message.ToString(&output);
    }
}

void TestProtobufReflection(benchmark::BenchmarkState& state, const string& encoded)
{
    using namespace google::protobuf;

    for (int x = 0; x != state.max_x; ++x)
    {
        HFPBUserInfo userinfo;
        if (userinfo.ParseFromString(encoded))
        {
            const Reflection * ref = userinfo.GetReflection();
            const Descriptor * descriptor = userinfo.GetDescriptor();
            const FieldDescriptor * field_descriptor = descriptor->FindFieldByName("goods");

            int size = ref->FieldSize(userinfo, field_descriptor);
            for (int i = 0; i < size; ++i)
            {
                const Message & msg = ref->GetRepeatedMessage(userinfo, field_descriptor, i);
                const Descriptor * goods_descriptor = msg.GetDescriptor();
                const Reflection * goods_ref = msg.GetReflection();

                const FieldDescriptor * goods_field_descriptor = goods_descriptor->FindFieldByName("goods_id");
                uint32_t goods_id = goods_ref->GetUInt32(msg, goods_field_descriptor);
                goods_field_descriptor = goods_descriptor->FindFieldByName("goods_num");
                uint32_t goods_num = goods_ref->GetUInt32(msg, goods_field_descriptor);
            }
        }
        //string output;
        //userinfo.SerializeToString(&output);
    }
}

void TestPbc(benchmark::BenchmarkState& state, struct pbc_env * env, const CompactProtobuf::Slice & encoded)
{
    struct pbc_slice slice;
    slice.buffer = encoded.start;
    slice.len = encoded.end - encoded.start;

    for (int x = 0; x != state.max_x; ++x)
    {
        struct pbc_rmessage * r_msg = pbc_rmessage_new(env, "petlib.HFPBUserInfo", &slice);
        int size = pbc_rmessage_size(r_msg, "goods");
        for (int i = 0; i < size; ++i)
        {
            struct pbc_rmessage * p = pbc_rmessage_message(r_msg , "goods", i);
            uint32_t goods_id = pbc_rmessage_integer(p, "goods_id", i, NULL);
            uint32_t goods_num = pbc_rmessage_integer(p, "goods_num", i, NULL);
        }
        pbc_rmessage_delete(r_msg);
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3) return -1;
    CompactProtobuf::Environment env;
	struct pbc_env * cenv = pbc_new();
    {
        CompactProtobuf::Slice slice;
        if (false == ReadFile(argv[1], &slice)) return -1;
        bool ok = env.Register(slice);
        if (not ok) return -1;
        {
            struct pbc_slice sl;
            sl.buffer = slice.start;
            sl.len = slice.end - slice.start;
            int ret = pbc_register(cenv, &sl);
            assert (ret == 0);
        }
        delete [] slice.start;
    }
    CompactProtobuf::Slice slice;
    const char * filename = argv[2];
    
    if (ReadFile(filename, &slice) == false) return -2;
    //printf("start: %p\tend: %p\n", slice.start, slice.end);

    string encoded(reinterpret_cast<char*>(slice.start), slice.end - slice.start);


    const size_t kMaxTimes = 1 << 12;
    benchmark::AddBench("Static", 50, kMaxTimes, 0, 0, boost::bind(TestStaticProtobuf, _1, encoded), NULL, NULL);
    benchmark::AddBench("Reflection", 50, kMaxTimes, 0, 0, boost::bind(TestProtobufReflection, _1, encoded), NULL, NULL);
    benchmark::AddBench("Dynamic", 50, kMaxTimes, 0, 0, boost::bind(TestDynamicProtobuf, _1, boost::ref(env), slice), NULL, NULL);
    benchmark::AddBench("PBC", 50, kMaxTimes, 0, 0, boost::bind(TestPbc, _1, cenv, slice), NULL, NULL);
    benchmark::ExecuteAll();

    //const CompactProtobuf::Descriptor * descriptor = env.FindMessageTypeByName("petlib.HFPBUserInfo");
    //if (not descriptor)
    //{
    //    fprintf(stderr, "FindMessageTypeByName HFPBUserInfo faield\n");
    //    return -3;
    //}

    //CompactProtobuf::Message message(descriptor);
    //bool ok = message.Init(slice);
    //if (not ok) return -3;

    //for (size_t i = 0; i != message.GetFieldSize("goods"); ++i)
    //{
    //    CompactProtobuf::Message * goods_msg = message.GetMessage("goods", i);
    //    assert (goods_msg);
    //    uint32_t goods_id = goods_msg->GetInteger("goods_id", 0, NULL);
    //    uint32_t goods_num = goods_msg->GetInteger("goods_num", 0, NULL);
    //    printf("good_id = %u\tgoods_num = %u\n", goods_id, goods_num);
    //}

    //{
    //    uint32_t low, flag;
    //    for (size_t i = 0; i != message.GetFieldSize("test"); ++i)
    //    {
    //        CompactProtobuf::Message * msg = message.GetMessage("test", i);
    //        //low = msg->GetInteger("idx", 0, NULL);
    //        //flag = msg->GetInteger("flag", 0, NULL);
    //        msg->Clear();
    //        msg->AddInteger("idx", i*2, 0);

    //        //printf("i = %4lu    low = %4d    flag = %s\n", i, low, BoolToString(flag));
    //    }
    //}

    //{
    //}

    //{
    //    string output;
    //    bool bok = message.ToString(&output);
    //    //printf("ToString %s!\n", bok ? "true" : "false");
    //    if (bok)
    //    {
    //        WriteFile("/tmp/message.dump", output);
    //    }
    //    else
    //    {
    //        printf("ToString failed!\n");
    //    }
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
