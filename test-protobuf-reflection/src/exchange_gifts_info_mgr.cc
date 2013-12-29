/*
 * =====================================================================================
 *
 *       Filename:  exchange_gifts_info_mgr.cc
 *        Created:  12/29/2013 07:28:48 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */
#include "exchange_gifts_info_mgr.h"

SINGLETON_IMPL(ExchangeGiftsInfoMgr);

ExchangeGiftsInfoMgr::ExchangeGiftsInfoMgr() {} 

int ExchangeGiftsInfoMgr::GetExchangeGiftsInfo(unsigned uin, ExchangeGiftsInfo* pInfo)
{
    assert( pInfo != NULL );
    InfoMap::iterator iter = this->infos_.find(uin);
    int ret = -1;
    if( iter != this->infos_.end() )
    {
        pInfo->CopyFrom( iter->second );
        ret = 0;
    }
    return ret;
}

int ExchangeGiftsInfoMgr::UpdateExchangeGiftsInfo(const ExchangeGiftsInfo& info)
{
    unsigned uin = info.uin();
    InfoMap::iterator iter = this->infos_.find( uin );
    if( iter != this->infos_.end() )
    {
        iter->second.CopyFrom( info );
    }
    else
    {
        this->infos_.insert( std::make_pair(uin, info) );
    }

    return 0;
}
