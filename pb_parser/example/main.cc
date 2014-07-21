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

//#include <google/profiler.h>
#include <stdint.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "compact_protobuf.h"
//#include "protobuf_helper.h"
//#include "protobuf_parser.h"

#include "benchmark.h"
#include "userinfo.pb.h"
#include "fx_object_pool.hpp"

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
        //    for (int i = 0; i != userinfo.goods_size(); ++i)
        //    {
        //        const HFPBUserGoods& goods = userinfo.goods(i);
        //        uint32_t goods_id = goods.goods_id();
        //        uint32_t goods_num = goods.goods_num();
        //    }
        }
        string output;
        userinfo.SerializeToString(&output);
    }
}

//void TestDynamicProtobuf(benchmark::BenchmarkState& state, const CompactProtobuf::Environment& env, const std::string& encoded)
//{
//    using namespace CompactProtobuf;
//    for (int x = 0; x != state.max_x; ++x)
//    {
//        //TRACE_TIME_CONSUME;
//        const CompactProtobuf::Descriptor * descriptor = env.FindMessageTypeByName("petlib.HFPBUserInfo");
//        CompactProtobuf::Message message(descriptor);
//        message.FromString(encoded);
//
//        int size = message.GetFieldSize("goods");
//        for (int i = 0; i != size; ++i)
//        {
//            CompactProtobuf::Message * goods_msg = message.GetMessage("goods", i);
//            uint32_t goods_id = goods_msg->GetInteger("goods_id", 0, NULL);
//            uint32_t goods_num = goods_msg->GetInteger("goods_num", 0, NULL);
//        }
//
//        //string output;
//        //message.ToString(&output);
//    }
//
//    //printf("Field copied %d times\n", Field::times);
//    //if (Value::copied_times != 0)
//    //{
//    //    printf("Value copied %d times\n", Value::copied_times);
//    //    Value::copied_times = 0;
//    //}
//    //printf("ParserState create %d times\n", ProtobufParser::ParserState::obj_count);
//    //ProtobufParser::ParserState::obj_count = 0;
//        printf("Field create %d times\n", Field::create_times);
//        printf("Value create %d times\n", Value::create_times);
//        Field::create_times = 0;
//        Value::create_times = 0;
//}

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

//void TestAppendValue(benchmark::BenchmarkState& state)
//{
//    CompactProtobuf::Field field;
//    for (int x = 0; x != state.max_x; ++x)
//    {
//        CompactProtobuf::Value v;
//        field.FastAppend (v);
//    }
//}
//
//void TestNewField(benchmark::BenchmarkState& state)
//{
//    for (int x = 0; x != state.max_x; ++x)
//    {
//        CompactProtobuf::Field * field = new CompactProtobuf::Field;
//        delete field;
//    }
//}

template<typename T>
void TestPoolObject(benchmark::BenchmarkState& state)
{
    fx::ObjectPool<T> pool;
    for (int x = 0; x != state.max_x; ++x)
    {
        T * p = pool.Construct();
        //pool.Destroy(p);
    }
}

//void TestNewValue(benchmark::BenchmarkState& state)
//{
//    for (int x = 0; x != state.max_x; ++x)
//    {
//        CompactProtobuf::Value * v = new CompactProtobuf::Value;
//        delete v;
//    }
//}

int main(int argc, char* argv[])
{
    if (argc != 3) return -1;
    //CompactProtobuf::Environment env;
	struct pbc_env * cenv = pbc_new();
    {
        CompactProtobuf::Slice slice;
        if (false == ReadFile(argv[1], &slice)) return -1;
        //bool ok = env.Register(slice);
        //if (not ok) return -1;
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

    string encoded(reinterpret_cast<char*>(slice.start), slice.end - slice.start);

    const size_t kMaxTimes = 1 << 10;
    //ProfilerStart("run.prof");
    benchmark::AddBench("Static", 50, kMaxTimes, 0, 0, boost::bind(TestStaticProtobuf, _1, encoded), NULL, NULL);
    //benchmark::AddBench("Reflection", 50, kMaxTimes, 0, 0, boost::bind(TestProtobufReflection, _1, encoded), NULL, NULL);
    //benchmark::AddBench("Dynamic", 50, kMaxTimes, 0, 0, boost::bind(TestDynamicProtobuf, _1, boost::ref(env), encoded), NULL, NULL);
    //benchmark::AddBench("TestConstructState", 50, kMaxTimes, 0, 0, TestConstructState, NULL, NULL);
    //benchmark::AddBench("NewField", 18350, 375808, 0, 0, TestNewField, NULL, NULL);
    //benchmark::AddBench("PoolField", 18350, 375808, 0, 0, TestPoolObject<CompactProtobuf::Field>, NULL, NULL);
    //benchmark::AddBench("NewValue", 39000, 798720, 0, 0, TestNewValue, NULL, NULL);
    //benchmark::AddBench("PoolValue", 39000, 798720, 0, 0, TestPoolObject<CompactProtobuf::Value>, NULL, NULL);
    benchmark::AddBench("PBC", 50, kMaxTimes, 0, 0, boost::bind(TestPbc, _1, cenv, slice), NULL, NULL);
    benchmark::ExecuteAll();
    //ProfilerStop();

    delete [] slice.start;
    return 0;
}
