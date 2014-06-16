/*
 * =====================================================================================
 *
 *       Filename:  unittest_environment.cc
 *        Created:  06/15/14 15:26:12
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "compact_protobuf.h"
#include <gtest/gtest.h>

using namespace CompactProtobuf;
using namespace google::protobuf;

class EnvironmentTest : public ::testing::Test
{
    protected:
        virtual void SetUp()
        {
            empty_slice_.start = NULL;
            empty_slice_.end = NULL;

            FileDescriptorSet protos;
            FileDescriptorProto * file = protos.add_file();
            file->set_name("foo.proto");

            DescriptorProto * message_proto = file->add_message_type();
            message_proto->set_name("Pair");

            FieldDescriptorProto *field_proto = NULL;
             
            field_proto = message_proto->add_field();
            field_proto->set_name("key");
            field_proto->set_type(FieldDescriptorProto::TYPE_STRING);
            field_proto->set_number(1);
            field_proto->set_label(FieldDescriptorProto::LABEL_REQUIRED);
             
            field_proto = message_proto->add_field();
            field_proto->set_name("value");
            field_proto->set_type(FieldDescriptorProto::TYPE_UINT32);
            field_proto->set_number(2);
            field_proto->set_label(FieldDescriptorProto::LABEL_REQUIRED);

            string output;
            EXPECT_EQ(true, protos.SerializeToString(&output));

            descriptor_slice_.start = reinterpret_cast<Byte*>(const_cast<char*>(output.data()));
            descriptor_slice_.end = descriptor_slice_.start + output.size();
        }

        //virtual void TearDown() { }

        Environment env_;
        Slice empty_slice_;
        Slice descriptor_slice_;
};

TEST_F(EnvironmentTest, TestRegister)
{
    EXPECT_EQ(false, env_.Register(empty_slice_));
    EXPECT_EQ(true, env_.Register(descriptor_slice_));
}

TEST_F(EnvironmentTest, TestFindMessageTypeByName)
{
    EXPECT_EQ(false, env_.Register(empty_slice_));
    EXPECT_EQ(true, env_.Register(descriptor_slice_));

    EXPECT_EQ(NULL, env_.FindMessageTypeByName("foo"));
    EXPECT_EQ(NULL, env_.FindMessageTypeByName("bar"));
    EXPECT_TRUE(NULL != env_.FindMessageTypeByName("Pair"));
}
