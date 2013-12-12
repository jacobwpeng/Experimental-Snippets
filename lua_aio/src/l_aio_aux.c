/*
 * =====================================================================================
 *
 *       Filename:  l_aio_aux.c
 *        Created:  12/12/2013 05:05:12 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "l_aio.h"
void dump_state(lua_State* L)
{
    int i;
    int top = lua_gettop(L);
    for( i = 1; i != top + 1; ++i)
    {
        printf("index = %d, type : %s\n", i, luaL_typename(L, i));
        if( lua_isstring(L, i) ) printf("value : %s\n", lua_tostring(L, i));
    }
    printf("********************************************************************************\n");
}

info_t* new_info()
{
    return (info_t*) malloc( sizeof(info_t) );
}

void delete_info(info_t* info)
{
    free( info );
}

