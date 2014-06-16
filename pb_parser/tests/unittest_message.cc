/*
 * =====================================================================================
 *
 *       Filename:  unittest_message.cc
 *        Created:  06/15/14 19:14:25
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "compact_protobuf.h"
#include <cstdlib>
#include <ctime>
#include <limits>
#include <vector>

#include <boost/assign.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#include <gtest/gtest.h>

using namespace CompactProtobuf;
using namespace google::protobuf;
using namespace boost::assign;
using std::vector;
using std::string;
using std::numeric_limits;

template<typename TargetType>
void GenerateRandomNumber(int num, vector<TargetType>* container)
{
    using namespace boost::assign;
    using std::numeric_limits;
    size_t seed = 1234567890;
    boost::random::mt19937 engine(seed);
    boost::random::uniform_real_distribution<> urd(numeric_limits<TargetType>::min(), numeric_limits<TargetType>::max());
    for (int i = 0; i < num; ++i) 
    {
        *container += urd(engine);
    }
}

class MessageTest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            MakeTestData();
            MakeTestProto();

            Slice slice;
            slice.start = reinterpret_cast<Byte*>( const_cast<char*>(encoded_.data()));
            slice.end = slice.start + encoded_.size();
            ASSERT_EQ(true, env_.Register(slice));
        }

        void MakeTestData()
        {
            const int kRandomNumberCount = 100;
            srand(time(NULL));

#define GENERATE_TEST_DATA(name, type, special_first, special_second, special_thrid)                                                 \
            name += special_first, special_second, special_thrid, numeric_limits<type>::min(), numeric_limits<type>::max();         \
            GenerateRandomNumber(100, &name);

            GENERATE_TEST_DATA(int32_list_, int32_t, 0, -1, 1);
            GENERATE_TEST_DATA(uint32_list_, uint32_t, 0, 1, 2);
            GENERATE_TEST_DATA(int64_list_, int64_t, 0, -1, 1);
            GENERATE_TEST_DATA(uint64_list_, uint64_t, 0, 1, 2);
            GENERATE_TEST_DATA(double_list_, double, 0, 0.1, -0.99999);
            GENERATE_TEST_DATA(float_list_, float, 0, 0.1, -0.99999);
#undef GENERATE_TEST_DATA

            string_list_ += "";
            const size_t kMaxStringLength = 1024;
            char buf[kMaxStringLength];
            FILE * fp = fopen("/dev/urandom", "r");
            ASSERT_TRUE( NULL != fp );
            for (int i = 0; i != kRandomNumberCount; ++i)
            {
                int len = rand() % kMaxStringLength;
                if (len < 0) len = -len;
                if (len == 0) len = 123;
                fread(buf, 1, len, fp);
                string_list_ += std::string(buf, len);
            }
            fclose(fp);
        }

        void MakeTestProto()
        {
            FileDescriptorSet protos;
            FileDescriptorProto * file = protos.add_file();
            file->set_name("testproto.proto");

            DescriptorProto * message_proto = file->add_message_type();
            message_proto->set_name("All");

            FieldDescriptorProto *field_proto = NULL;
            int field_number = 0;

#define ADD_FIELD(name, type, label)                                                 \
            field_proto = message_proto->add_field();                         \
            field_proto->set_name(name);                                      \
            field_proto->set_type(FieldDescriptorProto::TYPE_##type);         \
            field_proto->set_number(++field_number);                          \
            field_proto->set_label(FieldDescriptorProto::LABEL_##label);
             
            ADD_FIELD("i32", INT32, REQUIRED);
            ADD_FIELD("u32", UINT32, REQUIRED);
            ADD_FIELD("i64", INT64, REQUIRED);
            ADD_FIELD("u64", UINT64, REQUIRED);
            ADD_FIELD("s32", SINT32, REQUIRED);
            ADD_FIELD("s64", SINT64, REQUIRED);
            ADD_FIELD("sf32", SFIXED32, REQUIRED);
            ADD_FIELD("sf64", SFIXED64, REQUIRED);
            ADD_FIELD("s", STRING, REQUIRED);
            ADD_FIELD("d", DOUBLE, REQUIRED);
            ADD_FIELD("f", FLOAT, REQUIRED);
            ADD_FIELD("b", BOOL, REQUIRED);
#undef ADD_FIELD

            EXPECT_EQ(true, protos.SerializeToString(&encoded_));
        }

        Environment env_;
        vector<int32_t> int32_list_;
        vector<int64_t> int64_list_;
        vector<uint32_t> uint32_list_;
        vector<uint64_t> uint64_list_;
        vector<string> string_list_;
        vector<double> double_list_;
        vector<float> float_list_;
        string encoded_;
};

TEST_F(MessageTest, TestFromString)
{
}
