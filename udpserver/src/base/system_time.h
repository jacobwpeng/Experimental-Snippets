/*
 * =====================================================================================
 *
 *       Filename:  system_time.h
 *        Created:  08/22/14 20:41:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __SYSTEM_TIME_H__
#define  __SYSTEM_TIME_H__

#include <stdint.h>

namespace fx
{
    namespace base
    {
        namespace time
        {
            typedef uint64_t TimeStamp;

            //Unix timestamp in us
            TimeStamp Now();
        }
    }
}

#endif   /* ----- #ifndef __SYSTEM_TIME_H__  ----- */
