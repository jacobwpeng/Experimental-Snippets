/*
 * =====================================================================================
 *
 *       Filename:  userdata_wrapper.h
 *        Created:  07/24/14 14:30:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __USERDATA_WRAPPER__
#define  __USERDATA_WRAPPER__

#include <stdint.h>

struct UserdataWrapper
{
    struct {
        bool is_const;
        bool needs_gc;
        int64_t hash;
    } type_info;

    void * val;
};

#endif   /* ----- #ifndef __USERDATA_WRAPPER__  ----- */
