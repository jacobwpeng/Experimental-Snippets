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

TEST(Protobuf, MessageSize)
{
    ExchangePresentsInfo info;
    info.set_uin( 2191195 );
    info.set_last_login_time( 0 );
    info.set_unprocessed_message_count( 0 );

    for( unsigned idx = 0; idx != 5; ++idx )
    {
        SendRecord* p = info.mutable_send_records()->Add();
        p->set_uin( 1234567890 );
        p->set_type( RESPONSE_REQUEST );
    }

    for( unsigned idx = 0; idx != 5; ++idx )
    {
        ReceiveRecord * p = info.mutable_receive_records()->Add();
        p->set_uin( 1234567890 );
        p->set_type( BY_OTHER_ACTIVE );
    }

    for( unsigned idx = 0; idx != 30; ++idx )
    {
        RequestRecord * p = info.mutable_request_records()->Add();
        p->set_uin( 1234567890 );
    }

    for( unsigned idx = 0; idx != 25; ++idx )
    {
        Notification  * p = info.mutable_notifications()->Add();
        p->set_uin( 1234567890 );
        p->set_type( REQUEST_GIFT );
        GoodsInfo * pGoodsInfo = p->mutable_goods_list()->Add();
        pGoodsInfo->set_id( 3000 );
        pGoodsInfo->set_count( 1 );
    }

    cout << info.ByteSize() << '\n';
}
