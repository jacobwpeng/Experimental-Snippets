/*
 * =====================================================================================
 *
 *       Filename:  exchange_gifts.cc
 *        Created:  12/29/2013 07:36:35 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */
#include "exchange_gifts.h"
#include "exchange_gifts_info_mgr.h"
#include "exchange_gifts_detail.hpp"
#include "exchange_gifts_info.pb.h"

using namespace google::protobuf;

ExchangeGifts::ExchangeGifts()
    :max_day_request_num_(30),
     max_day_request_num_to_same_one_(1),
     max_day_send_num_(5),
     max_day_send_num_to_same_one_(1),
     max_day_receive_num_(5),
     max_day_receive_num_from_same_one_(1),
     max_request_gift_notification_num_(10),
     max_receive_gift_notification_num_(15),
     notification_ttl_( 5 * 3600 * 24 )         /* 5 days in seconds */
{
    //this->gifts_[10].push_back( { 3002, 1 } );
    //this->gifts_[20].push_back( { 3003, 1 } );
    //this->gifts_[30].push_back( { 3004, 1 } );
    //this->gifts_[40].push_back( { 3005, 1 } );
    //this->gifts_[50].push_back( { 3006, 1 } );
    //this->gifts_[999].push_back( {30007, 1} );
}

int ExchangeGifts::GetExchangeGiftsInfo(unsigned uin, ExchangeGiftsInfo* pInfo)
{
    int ret = ExchangeGiftsInfoMgrInst->GetExchangeGiftsInfo(uin, pInfo);
    if( ret == -1 )                             /* not found */
    {
        ExchangeGiftsInfo tmp;
        pInfo->Swap(&tmp);
        return 0;
    }
    else
    {
        return ret;
    }
}

int ExchangeGifts::UpdateExchangeGiftsInfo(const ExchangeGiftsInfo& info)
{
    return ExchangeGiftsInfoMgrInst->UpdateExchangeGiftsInfo(info);
}

void ExchangeGifts::ClearInfoIfExpired(unsigned uin)
{
    ExchangeGiftsInfo info;
    int ret = this->GetExchangeGiftsInfo(uin, &info);
    if( ret != 0 ) return;                      /* read error */

    time_t now = time(NULL);
    if( info.last_login_time() - now > 24 * 3600 ) /* one day in seconds */
    {
        /* clear info and reset last login time */
        RepeatedPtrField<Notification> notifications;
        notifications.Swap( info.mutable_notifications() );
        detail::RemoveExpiredNotifications( &notifications, this->notification_ttl_, now);

        info.Clear();
        info.set_last_login_time( now );
        info.mutable_notifications()->Swap( &notifications );

        ret = this->UpdateExchangeGiftsInfo( info );
        if( ret != 0 ) return;                  /* save error */
    }
}

int ExchangeGifts::AskForGifts(unsigned initiator_uin, unsigned recipient_uin)
{
    ExchangeGiftsInfo initiator_info;
    int ret = this->GetExchangeGiftsInfo(initiator_uin, &initiator_info);
    if( ret != 0 ) return ret;                      /* read error */

    if( initiator_info.request_records().size() > this->max_day_request_num_ ) return -1;
    if( detail::ReachRequestToSameUinLimit(initiator_info.request_records(), recipient_uin, this->max_day_request_num_to_same_one_) ) return -2;

    ExchangeGiftsInfo recipient_info;
    ret = this->GetExchangeGiftsInfo(recipient_uin, &recipient_info);
    if( ret != 0 ) return ret;                      /* read error */

    /* 判断对方的发送上限 */
    if( recipient_info.send_records().size() > this->max_day_send_num_ ) return -3; /* reach send limit */
    /* 对方今天不能再给自己送，是否需要特殊处理剩余次数 */
    if( detail::ReachSendToSameUinLimit(initiator_info.send_records(), recipient_uin, this->max_day_send_num_to_same_one_ ) ) return -4;
    
    /* 添加发起方请求记录 */
    RequestRecord * pRequestRecord = initiator_info.mutable_request_records()->Add();
    pRequestRecord->set_uin( recipient_uin );

    /* 添加接收方notifications */
    Notification* pNotification = detail::NewNotification(recipient_info.mutable_notifications(), REQUEST_GIFT, this->max_request_gift_notification_num_);
    pNotification->set_uin( initiator_uin ); /* 写接收方的notification,所以uin是发送方的 */
    unsigned last_notification_id = detail::IncreaseAddNotificationId(&recipient_info);
    pNotification->set_id( last_notification_id );
    pNotification->set_create_time( time(NULL) );

    unsigned level = 35;                        /* 接收请求方的等级，先写死 */

    /* 写入礼物信息 */
    GiftsMap::iterator iter = this->gifts_.lower_bound( level );
    assert( iter != this->gifts_.end() );
    detail::CopyGiftInfo( pNotification->mutable_goods_list(), iter->second );

    ret = this->UpdateExchangeGiftsInfo( initiator_info );
    if( ret != 0 ) return ret;                  /* update error */

    ret = this->UpdateExchangeGiftsInfo( recipient_info );
    if( ret != 0 ) return ret;                  /* update error */

    return 0;
}

int ExchangeGifts::ReplyGiftRequest(unsigned self_uin, unsigned notification_id)
{
    ExchangeGiftsInfo self_info;
    int ret = this->GetExchangeGiftsInfo(self_uin, &self_info);
    if( ret != 0 ) return ret;                      /* read error */

    if( self_info.send_records().size() > this->max_day_send_num_ ) return -1; /* reach send limit */

    Notification n;
    ret = detail::FindNotificationAndRemove( self_info.mutable_notifications(), notification_id, &n); /* reach send limit to same uin */
    if( ret != 0 ) return ret;                      /* read notification error */

    unsigned recipient_uin = n.uin();
    if( detail::ReachSendToSameUinLimit(self_info.send_records(), recipient_uin, this->max_day_send_num_to_same_one_ ) ) return -2;

    ExchangeGiftsInfo recipient_info;
    ret = this->GetExchangeGiftsInfo(recipient_uin, &recipient_info);
    if( ret != 0 ) return ret;                      /* read error */

    /* all conditions met*/
    /* first add self send record */
    SendRecord * pSendRecord = self_info.mutable_send_records()->Add();
    pSendRecord->set_uin( recipient_uin );
    pSendRecord->set_type( RESPONSE_REQUEST );
    /* then add recipient notifications */
    Notification * pNotification = detail::NewNotification( recipient_info.mutable_notifications(), RECEIVE_GIFT, this->max_receive_gift_notification_num_ );
    pNotification->set_uin( self_uin );
    pNotification->set_id( detail::IncreaseAddNotificationId(&recipient_info) );
    pNotification->set_create_time( time(NULL) );
    pNotification->mutable_goods_list()->Swap( n.mutable_goods_list() );
    /* then update unprocessed message count */
    recipient_info.set_unprocessed_message_count( recipient_info.unprocessed_message_count() + 1 );
    /* update data */
    ret = this->UpdateExchangeGiftsInfo(self_info);
    if( ret != 0 ) return ret;

    ret = this->UpdateExchangeGiftsInfo(recipient_info);
    if( ret != 0 ) return ret;

    return 0;
}

int ExchangeGifts::SendGifts(unsigned self_uin, unsigned recipient_uin)
{
    ExchangeGiftsInfo self_info;
    int ret = this->GetExchangeGiftsInfo(self_uin, &self_info);
    if( ret != 0 ) return ret;                      /* read error */

    /* check send limit */
    if( self_info.send_records().size() > this->max_day_send_num_ ) return -1; /* reach send limit */
    if( detail::ReachSendToSameUinLimit(self_info.send_records(), recipient_uin, this->max_day_send_num_to_same_one_) ) return -2;

    ExchangeGiftsInfo recipient_info;
    ret = this->GetExchangeGiftsInfo(recipient_uin, &recipient_info);
    if( ret != 0 ) return ret;                  /* read error */
    /* check recipient receive limit */
    //if( recipient_info.receive_records().size() > this->max_day_receive_num_ ) return -3;
    //if( detail::ReachReceiveFromSameUinLimit(recipient_info, self_uin, this->max_day_receive_num_from_same_one_) ) return -4;

    /* all conditions met */
    /* first add self send record */
    SendRecord * pSendRecord = self_info.mutable_send_records()->Add();
    pSendRecord->set_uin( recipient_uin );
    pSendRecord->set_type( SELF_ACTIVE );
    /* then add recipient notifications */
    Notification * pNotification = detail::NewNotification( recipient_info.mutable_notifications(), RECEIVE_GIFT, this->max_receive_gift_notification_num_ );
    pNotification->set_uin( self_uin );
    pNotification->set_id( detail::IncreaseAddNotificationId(&recipient_info) );
    pNotification->set_create_time( time(NULL) );

    unsigned self_level = 35;                        /* 自己的等级，先写死 */

    /* 写入礼物信息 */
    GiftsMap::iterator iter = this->gifts_.lower_bound( self_level );
    assert( iter != this->gifts_.end() );
    detail::CopyGiftInfo( pNotification->mutable_goods_list(), iter->second );
    /* then update unprocessed message count*/
    recipient_info.set_unprocessed_message_count( recipient_info.unprocessed_message_count() + 1 );
    /* update data */
    ret = this->UpdateExchangeGiftsInfo( self_info );
    if( ret != 0 ) return ret;                  /* update error */

    ret = this->UpdateExchangeGiftsInfo( recipient_info );
    if( ret != 0 ) return ret;                  /* update error */

    return 0;
}

int ExchangeGifts::ReceiveGift(unsigned self_uin, unsigned notification_id)
{
    ExchangeGiftsInfo self_info;
    int ret = this->GetExchangeGiftsInfo(self_uin, &self_info);
    if( ret != 0 ) return ret;                      /* read error */

    Notification n;
    ret = detail::FindNotificationAndRemove( self_info.mutable_notifications(), notification_id, &n); /* reach send limit to same uin */
    if( ret != 0 ) return ret;                      /* read notification error */

    unsigned sender_uin = n.uin();

    /* check self receive limit */
    if( self_info.receive_records().size() > this->max_day_receive_num_ ) return -1;
    if( detail::ReachReceiveFromSameUinLimit(self_info.receive_records(), sender_uin, this->max_day_receive_num_from_same_one_) ) return -2;

    /* all condtions met */
    /* add self receive record */
    ReceiveRecord * pReceiveRecord = self_info.mutable_receive_records()->Add();
    pReceiveRecord->set_uin(sender_uin);
    /* add self goods */
    //AddGoods()
    /* update data */
    ret = this->UpdateExchangeGiftsInfo( self_info );
    if( ret != 0 ) return ret;                  /* update error */
    return 0;
}
