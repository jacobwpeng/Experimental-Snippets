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
#include <numeric>
#include <set>
#include <iostream>
#include <vector>
#include <string>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <google/protobuf/message.h>
#include <google/protobuf/reflection_ops.h>
#include <gtest/gtest.h>
#include "exchange_gifts_info.pb.h"

using namespace std;
using namespace google::protobuf;

namespace detail
{
    bool CompareNotificationByCreateTime(const Notification& lhs, const Notification& rhs)
    {
        return lhs.create_time() < rhs.create_time();
    }

    bool LocateNotificationById(const Notification& notification, unsigned id)
    {
        return notification.id() == id;
    }
}

Notification* FindEarliestNotification(RepeatedPtrField<Notification>* notifications)
{
    RepeatedPtrField<Notification>::iterator iter = min_element( notifications->begin(), notifications->end(), detail::CompareNotificationByCreateTime );
    if( iter == notifications->end() ) return NULL;
    return &(*iter);
}

Notification* FindNotificationById(RepeatedPtrField<Notification>* notifications, unsigned id)
{
    RepeatedPtrField<Notification>::iterator iter = find_if( notifications->begin(), notifications->end(), boost::bind(detail::LocateNotificationById, _1, id));
    if( iter == notifications->end() ) return NULL;
    return &(*iter);
}

TEST(Protobuf, MaxMessageSize)
{
    ExchangeGiftsInfo info;
    info.set_uin( 2191195u );
    info.set_last_login_time( 0u );
    info.set_last_notification_id( 0u );
    info.set_unprocessed_message_count( 0u );

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
        p->set_create_time( idx );
        GoodsInfo * pGoodsInfo = p->mutable_goods_list()->Add();
        pGoodsInfo->set_id( 3000 );
        pGoodsInfo->set_count( 1 );
    }

    info.Clear();

    //Notification * p = FindEarliestNotification(info.mutable_notifications());
    //assert( p != NULL );
    //cout << p->create_time() << endl;
    //p->set_id( 5u );
    //
    //p = FindNotificationById(info.mutable_notifications(), 5);
    //assert( p != NULL );
    //cout << p->create_time() << endl;
}
