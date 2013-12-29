/*
 * =====================================================================================
 *
 *       Filename:  exchange_gifts_detail.hpp
 *        Created:  12/29/2013 07:51:44 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <limits>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include "exchange_gifts_info.pb.h"

using std::numeric_limits;
using boost::bind;
using namespace google::protobuf;

namespace detail
{

    bool FindNotificationById(const Notification& n, unsigned id)
    {
        return n.id() == id;
    }

    bool NotificationNotExpired(const Notification& notification, unsigned ttl, time_t now)
    {
        return now - notification.create_time() < ttl;
    }

    void RemoveExpiredNotifications(RepeatedPtrField<Notification>* notifications, unsigned ttl, time_t now) /* ttl in seconds */
    {
        RepeatedPtrField<Notification>::iterator new_end = partition( notifications->begin(), notifications->end(),
                                                                      bind<bool>(NotificationNotExpired, _1, ttl, now) );
        RepeatedPtrField<Notification> tmp( notifications->begin(), new_end );
        notifications->Swap( &tmp );
    }

    unsigned IncreaseAddNotificationId(ExchangeGiftsInfo* pInfo)
    {
        unsigned last_id = pInfo->last_notification_id();
        pInfo->set_last_notification_id(last_id + 1); /* 32 bit unsigned, cannot overflow */
        return last_id;
    }

    void CopyGiftInfo(RepeatedPtrField<GoodsInfo>* pGifts, const vector<ExchangeGifts::GiftsInfo>& gifts_template)
    {
        assert( pGifts->size() == 0 );
        BOOST_FOREACH(const ExchangeGifts::GiftsInfo& one_gift, gifts_template)
        {
            GoodsInfo * p = pGifts->Add();
            p->set_id( one_gift.id );
            p->set_count( one_gift.count );
        }
    }

    Notification* NewNotification(RepeatedPtrField<Notification>* notifications, NotificationType type, unsigned limit_num) /* limit_num 本类notification上限值 */
    {
        assert( notifications != NULL );
        assert( limit_num > 0u );
        assert( type != NIL );
        RepeatedPtrField<Notification>::iterator iter = notifications->begin();
        RepeatedPtrField<Notification>::iterator earliest_notification_iter;
        RepeatedPtrField<Notification>::iterator empty_iter = notifications->end();

        time_t time = numeric_limits<time_t>::max();
        unsigned num = 0;
        while( iter != notifications->end() )
        {
            if( iter->type() == type )
            {
                ++num;
                if( iter->create_time() < time ) /* earlier notification */
                {
                    time = iter->create_time();
                    earliest_notification_iter = iter;
                }
            }
            else if( iter->type() == NIL )
            {
                empty_iter = iter;
            }
            ++iter;
        }

        if( num < limit_num )                   /* not reach limit */
        {
            Notification* p = NULL;
            if( empty_iter == notifications->end() ) 
            {
                p = notifications->Add();
            }
            else
            {
                p = &(*empty_iter);
            }
            return p;
        }
        else
        {
            earliest_notification_iter->Clear(); /* 清空这个notification */
            earliest_notification_iter->set_type( type );
            return &(*earliest_notification_iter);
        }
    }

    int FindNotificationAndRemove(RepeatedPtrField<Notification>* notifications, unsigned id, Notification* pRes)
    {
        RepeatedPtrField<Notification>::iterator iter = find_if( notifications->begin(), notifications->end(), bind(FindNotificationById, _1, id) );
        if( iter != notifications->end() )
        {
            pRes->CopyFrom( *iter );
            iter->Clear();
            iter->set_type(NIL);
            return 0;
        }
        return -1;
    }

    template<typename T>
    bool ReachToSameUinLimit( const RepeatedPtrField<T>& records, unsigned uin, unsigned limit_num )
    {
        typename RepeatedPtrField<T>::const_iterator iter = records.begin();
        unsigned num = 0;
        while( iter != records.end() )
        {
            if( iter->uin() == uin ) ++num;
            if( num > limit_num ) return true;
            ++iter;
        }
        return false;
    }

    bool ReachReceiveFromSameUinLimit(const RepeatedPtrField<ReceiveRecord>& receive_records, unsigned uin, unsigned limit_num)
    {
        return ReachToSameUinLimit(receive_records, uin, limit_num);
    }

    bool ReachSendToSameUinLimit(const RepeatedPtrField<SendRecord>& send_records, unsigned uin, unsigned limit_num)
    {
        return ReachToSameUinLimit(send_records, uin, limit_num);
    }

    bool ReachRequestToSameUinLimit(const RepeatedPtrField<RequestRecord>& request_records, unsigned uin, unsigned limit_num)
    {
        return ReachToSameUinLimit(request_records, uin, limit_num);
    }
}

