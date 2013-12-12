/*
 * =====================================================================================
 *
 *       Filename:  l_aio_aux.h
 *        Created:  12/12/2013 05:03:06 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  aux function for aio lib
 *
 * =====================================================================================
 */

#include <lua.h>

void dump_state(lua_State* L);
info_t* new_info();
void delete_info(info_t* info);
