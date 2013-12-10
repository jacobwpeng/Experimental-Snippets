/*
 * =====================================================================================
 *
 *       Filename:  l_array.c
 *        Created:  12/10/2013 09:25:33 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <signal.h>
#include <lauxlib.h>
#include "l_array.h"

static lua_State* global_L = NULL;

void sig_handle(int sig)
{
    if( sig != SIGINT ) return;
    fprintf(stderr, "receive SIGINT\n");
}

static int new_array(lua_State* L)
{
    global_L = L;
    int i;
    size_t nBytes;
    array* pArray;

    int n = luaL_checkint(L, 1);
    luaL_argcheck(L, n > 0, 1, "Invalid size");

    nBytes = sizeof(array) + sizeof(int) * n;
    pArray = (array*) lua_newuserdata(L, nBytes);

    pArray->size = n;
    pArray->arr = (int*)((char*)pArray + sizeof(array));
    for( i = 0; i != n; ++i )
    {
        pArray->arr[i] = 0;
    }

    luaL_getmetatable(L, "larray");
    lua_setmetatable(L, -2);
    return 1;
}

static int set_array(lua_State* L)
{
    int i, val;
    array* pArray;

    pArray = (array*)lua_touserdata(L, 1);
    luaL_argcheck(L, pArray != NULL, 1, "array expected");

    i = luaL_checkint(L, 2);
    luaL_argcheck(L, i > 0 && i <= pArray->size, 2, "invalid index");

    val = luaL_checkint(L, 3);
    pArray->arr[i-1] = val;
    return 0;
}

static int get_array(lua_State* L)
{
    int i, val;
    array* pArray;

    pArray = (array*)lua_touserdata(L, 1);
    luaL_argcheck(L, pArray != NULL, 1, "array expected");

    i = luaL_checkint(L, 2);
    luaL_argcheck(L, i > 0 && i <= pArray->size, 2, "invalid index");

    val = pArray->arr[i-1];
    lua_pushinteger(L, val);
    return 1;
    //return 1;
}

static int delete_array(lua_State* L)
{
    array* pArray = (array*)L;
    return 0;
}

static const struct luaL_Reg arraylib_f[] = {
    {"new", new_array},
    {NULL, NULL}
};

static const struct luaL_Reg arraylib_m[] = {
    {"set", set_array},
    {"get", get_array},
    {"__gc", delete_array},
    {NULL, NULL}
};

int luaopen_array(lua_State* L)
{
    signal(SIGINT, sig_handle);
    luaL_newmetatable(L, "larray");

    /* metatable.__index = metatable */
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_register(L, NULL, arraylib_m);
    luaL_register(L, "array", arraylib_f);
    return 1;
}
