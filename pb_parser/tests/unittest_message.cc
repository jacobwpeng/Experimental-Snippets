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
#include <cstdio>
#include <ctime>
#include <limits>
#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/assign.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <google/protobuf/dynamic_message.h>

#include <gtest/gtest.h>
#include "TestPacked.pb.h"

//using namespace CompactProtobuf;
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

            CompactProtobuf::Slice slice;
            slice.start = reinterpret_cast<CompactProtobuf::Byte*>( const_cast<char*>(encoded_.data()));
            slice.end = slice.start + encoded_.size();
            ASSERT_EQ(true, env_.Register(slice));
        }

        void MakeTestData()
        {
            const int kRandomNumberCount = 5;
            srand(time(NULL));

#define GENERATE_TEST_DATA(name, type, special_first, special_second, special_thrid)                                                 \
            name += special_first, special_second, special_thrid, numeric_limits<type>::min(), numeric_limits<type>::max();         \
            GenerateRandomNumber(kRandomNumberCount, &name);

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
            FileDescriptorProto * file = protos_.add_file();
            file->set_name("testproto.proto");

            DescriptorProto * sub_proto = file->add_message_type();
            sub_proto ->set_name("Sub");
            FieldDescriptorProto *field_proto = NULL;

            field_proto = sub_proto->add_field();                             
            field_proto->set_name("val");                                      
            field_proto->set_type(FieldDescriptorProto::TYPE_FLOAT);         
            field_proto->set_number(1);                          
            field_proto->set_label(FieldDescriptorProto::LABEL_REQUIRED);

            DescriptorProto * all_proto = file->add_message_type();
            all_proto->set_name("All");

            int field_number = 0;

#define ADD_FIELD(name, type, label)                                          \
            field_proto = all_proto->add_field();                             \
            field_proto->set_name(name);                                      \
            field_proto->set_type(FieldDescriptorProto::TYPE_##type);         \
            field_proto->set_number(++field_number);                          \
            field_proto->set_label(FieldDescriptorProto::LABEL_##label);
             
            ADD_FIELD("i32", INT32, REPEATED);
            ADD_FIELD("u32", UINT32, REPEATED);
            ADD_FIELD("i64", INT64, REPEATED);
            ADD_FIELD("u64", UINT64, REPEATED);
            ADD_FIELD("s32", SINT32, REPEATED);
            ADD_FIELD("s64", SINT64, REPEATED);
            ADD_FIELD("sf32", SFIXED32, REPEATED);
            ADD_FIELD("sf64", SFIXED64, REPEATED);
            ADD_FIELD("s", BYTES, REPEATED);
            ADD_FIELD("d", DOUBLE, REPEATED);
            ADD_FIELD("f", FLOAT, REPEATED);
            ADD_FIELD("b", BOOL, REPEATED);

            ADD_FIELD("m", MESSAGE, REPEATED);
            field_proto->set_type_name("Sub");
#undef ADD_FIELD

            EXPECT_EQ(true, protos_.SerializeToString(&encoded_));
            const FileDescriptor * file_descriptor = pool_.BuildFile(*file);
            const Descriptor * descriptor = file_descriptor->FindMessageTypeByName("All");

            DynamicMessageFactory factory(&pool_);
            const ::google::protobuf::Message * prototype = factory.GetPrototype(descriptor);
            
            message_ = prototype->New();

            {
                const Reflection * ref = message_->GetReflection();
                const FieldDescriptor * field = NULL;

#define FILL_REPEATED_DATA(message, name, list, type)              \
                field = descriptor->FindFieldByName(name);             \
                for (size_t i = 0 ; i != list.size(); ++i)             \
                {                                                      \
                    ref->Add##type(message, field, list[i]);           \
                }                                                     

                FILL_REPEATED_DATA(message_, "i32", int32_list_, Int32);
                FILL_REPEATED_DATA(message_, "u32", uint32_list_, UInt32);
                FILL_REPEATED_DATA(message_, "s32", int32_list_, Int32);
                FILL_REPEATED_DATA(message_, "sf32", int32_list_, Int32);

                FILL_REPEATED_DATA(message_, "i64", int64_list_, Int64);
                FILL_REPEATED_DATA(message_, "u64", uint64_list_, UInt64);
                FILL_REPEATED_DATA(message_, "s64", int64_list_, Int64);
                FILL_REPEATED_DATA(message_, "sf64", int64_list_, Int64);

                FILL_REPEATED_DATA(message_, "s", string_list_, String);
                FILL_REPEATED_DATA(message_, "d", double_list_, Double);
                FILL_REPEATED_DATA(message_, "f", float_list_, Float);
                FILL_REPEATED_DATA(message_, "b", int32_list_, Bool);

                ref = packed_.GetReflection();
                descriptor = packed_.GetDescriptor();
                FILL_REPEATED_DATA(&packed_, "i32", int32_list_, Int32);
                FILL_REPEATED_DATA(&packed_, "u32", uint32_list_, UInt32);
                FILL_REPEATED_DATA(&packed_, "s32", int32_list_, Int32);
                FILL_REPEATED_DATA(&packed_, "sf32", int32_list_, Int32);

                FILL_REPEATED_DATA(&packed_, "i64", int64_list_, Int64);
                FILL_REPEATED_DATA(&packed_, "u64", uint64_list_, UInt64);
                FILL_REPEATED_DATA(&packed_, "s64", int64_list_, Int64);
                FILL_REPEATED_DATA(&packed_, "sf64", int64_list_, Int64);

                FILL_REPEATED_DATA(&packed_, "d", double_list_, Double);
                FILL_REPEATED_DATA(&packed_, "f", float_list_, Float);
                FILL_REPEATED_DATA(&packed_, "b", int32_list_, Bool);
            }
#undef FILL_DATA

            const FieldDescriptor * field = message_->GetDescriptor()->FindFieldByName("m");
            EXPECT_TRUE (field != NULL);
            for (size_t i = 0; i != float_list_.size(); ++i)
            {
                ::google::protobuf::Message * sub = message_->GetReflection()->AddMessage(message_, field);
                const Reflection * subref = sub->GetReflection();
                const FieldDescriptor * sub_descriptor = sub->GetDescriptor()->FindFieldByName("val");
                subref->SetFloat(sub, sub_descriptor, float_list_[i]);
            }

            message_->SerializeToString(&encoded_message_);
            packed_.SerializeToString(&encoded_packed_message_);

        }

        void CheckValue(CompactProtobuf::Message& compact_message, bool check_bytes = true)
        {
#define CHECK_INTEGER_VALUE(name, list, type)                                           \
            {                                                                           \
                uint32_t low, hi;                                                       \
                ASSERT_EQ(compact_message.GetFieldSize(name), list.size());             \
                for (size_t i = 0; i != compact_message.GetFieldSize(name); ++i)        \
                {                                                                       \
                    low = compact_message.GetInteger(name, i, &hi);                     \
                    uint64_t val = hi;                                                  \
                    val <<= 32;                                                         \
                    val |= low;                                                         \
                    EXPECT_EQ( static_cast<type>(val), list[i] );                       \
                }                                                                       \
            }
            CHECK_INTEGER_VALUE("i32", int32_list_, int32_t);
            CHECK_INTEGER_VALUE("u32", uint32_list_, uint32_t);
            CHECK_INTEGER_VALUE("s32", int32_list_, int32_t);
            CHECK_INTEGER_VALUE("sf32", int32_list_, int32_t);

            CHECK_INTEGER_VALUE("i64", int64_list_, int64_t);
            CHECK_INTEGER_VALUE("u64", uint64_list_, uint64_t);
            CHECK_INTEGER_VALUE("s64", int64_list_, int64_t);
            CHECK_INTEGER_VALUE("sf64", int64_list_, int64_t);
#undef CHECK_INTEGER_VALUE

#define CHECK_VALUE(name, type, list)                                                   \
            {                                                                           \
                ASSERT_EQ(compact_message.GetFieldSize(name), list.size());             \
                for (size_t i = 0; i != compact_message.GetFieldSize(name); ++i)        \
                {                                                                       \
                    EXPECT_EQ( compact_message.Get##type(name, i), list[i] );           \
                }                                                                       \
            }
            if (check_bytes) CHECK_VALUE("s", String, string_list_);
            CHECK_VALUE("d", Real, double_list_);
            CHECK_VALUE("f", Real, float_list_);
#undef CHECK_VALUE

            {
                ASSERT_EQ (compact_message.GetFieldSize("b"), int32_list_.size());
                for (size_t i = 0; i != compact_message.GetFieldSize("b"); ++i)
                {
                    EXPECT_EQ (static_cast<bool>(compact_message.GetInteger("b", i, NULL)), int32_list_[i] != 0);
                }
            }
        }

        CompactProtobuf::Environment env_;
        vector<int32_t> int32_list_;
        vector<int64_t> int64_list_;
        vector<uint32_t> uint32_list_;
        vector<uint64_t> uint64_list_;
        vector<string> string_list_;
        vector<double> double_list_;
        vector<float> float_list_;
        string encoded_;
        string encoded_message_;
        string encoded_packed_message_;
        FileDescriptorSet protos_;
        DescriptorPool pool_;
        ::google::protobuf::Message * message_;
        Packed packed_;
};

TEST_F(MessageTest, TestFromString)
{
    const Descriptor * descriptor = env_.FindMessageTypeByName("All");
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message all(descriptor);
    ASSERT_TRUE (all.FromString(encoded_message_));
    CheckValue(all);
}

TEST_F(MessageTest, TestPacked)
{
    const Descriptor * descriptor = packed_.GetDescriptor();
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message packed(descriptor);
    ASSERT_TRUE (packed.FromString(encoded_packed_message_));
    CheckValue(packed, false);
}

TEST_F(MessageTest, TestToString)
{
    const Descriptor * descriptor = env_.FindMessageTypeByName("All");
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message all(descriptor);
    ASSERT_TRUE (all.FromString(encoded_message_));
    string encoded;
    all.ToString(&encoded);

    ASSERT_TRUE( encoded  == encoded_message_);
}

TEST_F(MessageTest, Test_has_field)
{
    const Descriptor * descriptor = env_.FindMessageTypeByName("All");
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message all(descriptor);
    ASSERT_TRUE (all.FromString(encoded_message_));

    EXPECT_TRUE (all.has_field());
    EXPECT_TRUE (all.has_field("i32"));
    EXPECT_TRUE (all.has_field("u32"));
    EXPECT_TRUE (all.has_field("i64"));
    EXPECT_TRUE (all.has_field("u64"));
    EXPECT_TRUE (all.has_field("s32"));
    EXPECT_TRUE (all.has_field("s64"));
    EXPECT_TRUE (all.has_field("sf32"));
    EXPECT_TRUE (all.has_field("sf64"));
    EXPECT_TRUE (all.has_field("s"));
    EXPECT_TRUE (all.has_field("d"));
    EXPECT_TRUE (all.has_field("f"));
    EXPECT_TRUE (all.has_field("b"));
    EXPECT_FALSE (all.has_field("foo"));
}

TEST_F(MessageTest, TestAdd)
{
    const Descriptor * descriptor = env_.FindMessageTypeByName("All");
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message all(descriptor);
    all.AddInteger("i32", 123, 0);
    EXPECT_EQ (1, all.GetFieldSize("i32"));
    EXPECT_EQ (123, all.GetInteger("i32", 0, NULL));
    EXPECT_EQ (0, all.GetFieldSize("i64"));

    all.AddInteger("s64", 789, 123);
    EXPECT_EQ (1, all.GetFieldSize("s64"));

    uint32_t low, hi;
    low = all.GetInteger("s64", 0, &hi);
    EXPECT_EQ (low, 789);
    EXPECT_EQ (hi, 123);

    all.AddString("s", "Hello");
    all.AddString("s", "World");
    EXPECT_EQ (2, all.GetFieldSize("s"));
    EXPECT_EQ ("Hello", all.GetString("s", 0));
    EXPECT_EQ ("World", all.GetString("s", 1));

    double d = 123.45;
    all.AddReal("d", d);
    EXPECT_TRUE (all.has_field("d"));
    EXPECT_EQ (1, all.GetFieldSize("d"));
    EXPECT_EQ (d, all.GetReal("d", 0));

}

TEST_F(MessageTest, TestSet)
{
    const Descriptor * descriptor = env_.FindMessageTypeByName("All");
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message all(descriptor);
    all.AddInteger("i32", 123, 0);
    all.SetInteger("i32", 0, 456, 0);
    EXPECT_EQ (456, all.GetInteger("i32", 0, NULL));

    string msg = "Hello, world!";
    all.AddString("s", msg);
    all.SetString("s", 0, "World");
    EXPECT_EQ ("World", all.GetString("s", 0));

    double d = 3.14159265358;
    all.AddReal("d", 0);
    all.SetReal("d", 0, d);
    EXPECT_EQ (d, all.GetReal("d", 0));

    float f = 9.26;
    EXPECT_FALSE (all.has_field("m"));

    CompactProtobuf::Message * sub = all.AddMessage("m");
    EXPECT_TRUE (all.has_field("m"));
    EXPECT_FALSE (sub->has_field("val"));

    sub->SetReal("val", 0, f);
    ASSERT_TRUE (sub->has_field("val"));

    EXPECT_EQ(f, sub->GetReal("val", 0));
}

TEST_F(MessageTest, TestDelete)
{
    const Descriptor * descriptor = env_.FindMessageTypeByName("All");
    ASSERT_TRUE (descriptor != NULL);

    CompactProtobuf::Message all(descriptor);
    all.AddInteger("i32", 123, 0);
    all.AddInteger("i32", 456, 0);
    EXPECT_EQ (123, all.GetInteger("i32", 0, NULL));
    EXPECT_EQ (456, all.GetInteger("i32", 1, NULL));

    EXPECT_TRUE (all.has_field("i32"));
    EXPECT_EQ (2, all.GetFieldSize("i32"));
    EXPECT_EQ (123, all.DeleteInteger("i32", 0, NULL));
    EXPECT_EQ (456, all.GetInteger("i32", 0, NULL));
    EXPECT_EQ (1, all.GetFieldSize("i32"));
    EXPECT_EQ (456, all.DeleteInteger("i32", 0, NULL));
    EXPECT_FALSE (all.has_field("i32"));
}
