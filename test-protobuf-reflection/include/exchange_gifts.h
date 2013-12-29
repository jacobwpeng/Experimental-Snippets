/*
 * =====================================================================================
 *
 *       Filename:  exchange_gifts.h
 *        Created:  12/29/2013 07:33:45 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __EXCHANGE_GIFTS__
#define  __EXCHANGE_GIFTS__

#include "singleton.hpp"
#include "exchange_gifts_info.pb.h"
#include <vector>
#include <map>

using std::vector;
using std::map;

class ExchangeGifts
{
    public:
        struct GiftsInfo { unsigned id; unsigned count; };

        ExchangeGifts();
        void ClearInfoIfExpired(unsigned uin);

        int AskForGifts(unsigned initiator_uin, unsigned recipient_uin); /* 索要礼物 */
        int SendGifts(unsigned initiator_uin, unsigned recipient_uin); /* 主动送礼物 */
        int ReplyGiftRequest(unsigned self_uin, unsigned notification_id); /* 回复别人索要请求 */
        int ReceiveGift(unsigned self_uin, unsigned notification_id); /* 收取礼物 */

        int GetExchangeGiftsInfo(unsigned uin, ExchangeGiftsInfo* pInfo);
        int UpdateExchangeGiftsInfo(const ExchangeGiftsInfo& info);
    private:
        const unsigned max_day_request_num_;
        const unsigned max_day_request_num_to_same_one_;
        const unsigned max_day_send_num_;
        const unsigned max_day_send_num_to_same_one_;
        const unsigned max_day_receive_num_;
        const unsigned max_day_receive_num_from_same_one_;
        const unsigned max_request_gift_notification_num_;
        const unsigned max_receive_gift_notification_num_;
        const unsigned notification_ttl_;

        /* level -> gifts map */
        typedef map<unsigned, vector<GiftsInfo> > GiftsMap;
        GiftsMap gifts_;
};

#endif   /* ----- #ifndef __EXCHANGE_GIFTS__  ----- */
