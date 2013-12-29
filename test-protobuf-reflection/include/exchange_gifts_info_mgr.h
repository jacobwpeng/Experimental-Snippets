/*
 * =====================================================================================
 *
 *       Filename:  exchange_gifts_info_mgr.h
 *        Created:  12/29/2013 07:21:32 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __EXCHANGE_GIFTS_INFO_MGR__
#define  __EXCHANGE_GIFTS_INFO_MGR__

#include "singleton.hpp"
#include "exchange_gifts_info.pb.h"
#include <map>

using std::map;

class ExchangeGiftsInfoMgr
{
    public:
        ExchangeGiftsInfoMgr();
        int GetExchangeGiftsInfo(unsigned uin, ExchangeGiftsInfo* pInfo);
        int UpdateExchangeGiftsInfo(const ExchangeGiftsInfo& info);

    private:
        typedef map<unsigned, ExchangeGiftsInfo> InfoMap;
        InfoMap infos_;
        ENABLE_SINGLETON(ExchangeGiftsInfoMgr);
};

#define ExchangeGiftsInfoMgrInst Singleton<ExchangeGiftsInfoMgr>::Instance()


#endif   /* ----- #ifndef __EXCHANGE_GIFTS_INFO_MGR__  ----- */
