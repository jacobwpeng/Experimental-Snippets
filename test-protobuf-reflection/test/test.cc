/*
 * =====================================================================================
 *
 *       Filename:  test.cc
 *        Created:  12/06/2013 10:08:15 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/reflection_ops.h>
#include <gtest/gtest.h>
#include "userinfo.pb.h"

using namespace std;
using namespace google::protobuf;

Message* GetMessageInternal(Message* pInfo, const string& name)
{
    const Reflection* reflection = pInfo->GetReflection();
    assert( reflection != NULL );

    const Descriptor* descriptor = pInfo->GetDescriptor();
    assert( descriptor != NULL );

    const FieldDescriptor* field_descriptor = descriptor->FindFieldByName(name);
    assert( field_descriptor != NULL );

    return reflection->MutableMessage(pInfo, field_descriptor);
}


template<typename T>
T* GetMessage(Message* pInfo, const string& name)
{
    vector<string> res;
    boost::split(res, name, boost::is_any_of("."));
    Message* p = pInfo;
    for( unsigned idx = 0; idx != res.size(); ++idx)
    {
        if( p == NULL ) break;
        p = GetMessageInternal(p, res[idx] );
    }
    assert( p != NULL );
    return dynamic_cast<T*>(p);
}

TEST(ReflectionTest, GetInstance)
{
    ActInfo info;
    ChristmasAct* p = GetMessage<ChristmasAct>(&info, "novice_leveling.christmas_act");
    EXPECT_TRUE( p != NULL );
}
