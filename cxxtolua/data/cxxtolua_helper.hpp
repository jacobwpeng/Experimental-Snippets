/*
 * =====================================================================================
 *
 *       Filename:  cxxtolua_helper.hpp
 *        Created:  08/15/14 10:29:31
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __CXXTOLUA_HELPER_HPP__
#define  __CXXTOLUA_HELPER_HPP__

#include <string>
#include <stdint.h>
#include <lua.hpp>

struct UserdataWrapper
{
    struct {
        bool is_const;
        bool needs_gc;
        int64_t hash;
    } type_info;
    void * val;
};

template <typename T>
bool CheckArgument(lua_State * L, int idx, T arg);

#define CHECK_SIGNED_INTEGER_TYPE(cpptype)\
template <>\
bool CheckArgument(lua_State * L, int idx, cpptype * arg)\
{\
    int isnum;\
    *arg = lua_tointegerx(L, idx, &isnum);\
    return isnum;\
}
//char always in range(int_min, int_max)
CHECK_SIGNED_INTEGER_TYPE(char)
CHECK_SIGNED_INTEGER_TYPE(signed char)
CHECK_SIGNED_INTEGER_TYPE(signed short int)
CHECK_SIGNED_INTEGER_TYPE(signed int)
CHECK_SIGNED_INTEGER_TYPE(signed long int)
CHECK_SIGNED_INTEGER_TYPE(signed long long int)
#undef CHECK_SIGNED_INTEGER_TYPE

#define CHECK_UNSIGNED_INTEGER_TYPE(cpptype)\
template <>\
bool CheckArgument(lua_State * L, int idx, cpptype * arg)\
{\
    int isnum;\
    *arg = lua_tounsignedx(L, idx, &isnum);\
    return isnum;\
}
CHECK_UNSIGNED_INTEGER_TYPE(unsigned char)
CHECK_UNSIGNED_INTEGER_TYPE(unsigned short int)
CHECK_UNSIGNED_INTEGER_TYPE(unsigned int)
CHECK_UNSIGNED_INTEGER_TYPE(unsigned long int)
CHECK_UNSIGNED_INTEGER_TYPE(unsigned long long int)
#undef CHECK_UNSIGNED_INTEGER_TYPE

#define CHECK_FLOATING_TYPE(cpptype)\
template <>\
bool CheckArgument(lua_State * L, int idx, cpptype * arg)\
{\
    int isnum;\
    *arg = lua_tonumberx(L, idx, &isnum);\
    return isnum;\
}

CHECK_FLOATING_TYPE(float)
CHECK_FLOATING_TYPE(double)
#undef CHECK_FLOATING_TYPE

template <>
bool CheckArgument(lua_State * L, int idx, bool * arg)
{
    *arg = (lua_toboolean(L, idx) != 0);
    return true;
}

template<typename T>
bool CheckArgument(lua_State * L, int idx, const T ** arg, int64_t hash)
{
    //check const userdata
    return true;
}

template<typename T>
bool CheckArgument(lua_State * L, int idx, T ** arg, int64_t hash)
{
    //check mutable userdata
    return true;
}

template <typename T>
int PushResult(lua_State * L, T res);

#define PUSH_SIGNED_INTEGER_TYPE(cpptype)\
template <>\
int PushResult(lua_State * L, cpptype res)\
{\
    lua_pushinteger(L, res);\
    return 1;\
}
PUSH_SIGNED_INTEGER_TYPE(char)
PUSH_SIGNED_INTEGER_TYPE(signed char)
PUSH_SIGNED_INTEGER_TYPE(signed short int)
PUSH_SIGNED_INTEGER_TYPE(signed int)
PUSH_SIGNED_INTEGER_TYPE(signed long int)
PUSH_SIGNED_INTEGER_TYPE(signed long long int)
#undef PUSH_SIGNED_INTEGER_TYPE

#define PUSH_UNSIGNED_INTEGER_TYPE(cpptype)\
template <>\
int PushResult(lua_State * L, cpptype res)\
{\
    lua_pushunsigned(L, res);\
    return 1;\
}
PUSH_UNSIGNED_INTEGER_TYPE(unsigned char)
PUSH_UNSIGNED_INTEGER_TYPE(unsigned short int)
PUSH_UNSIGNED_INTEGER_TYPE(unsigned int)
PUSH_UNSIGNED_INTEGER_TYPE(unsigned long int)
PUSH_UNSIGNED_INTEGER_TYPE(unsigned long long int)
#undef PUSH_UNSIGNED_INTEGER_TYPE

#define PUSH_FLOATING_TYPE(cpptype)\
template <>\
int PushResult(lua_State * L, cpptype res)\
{\
    lua_pushnumber(L, res);\
    return 1;\
}

PUSH_FLOATING_TYPE(float)
PUSH_FLOATING_TYPE(double)
#undef PUSH_FLOATING_TYPE

template <>
int PushResult(lua_State * L, const std::string& res)
{
    lua_pushlstring(L, res.data(), res.size());
    return 1;
}

template <>
int PushResult(lua_State * L, const char * res)
{
    lua_pushstring(L, res);
    return 1;
}

template <>
int PushResult(lua_State * L, bool res)
{
    lua_pushboolean(L, res);
    return 1;
}

template <typename T>
int PushResult(lua_State * L, const T * res, bool is_const, int64_t hashcode, const char * mt_name)
{
    UserdataWrapper * wrapper = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
    wrapper->type_info.is_const = is_const;
    wrapper->type_info.needs_gc = false;
    wrapper->type_info.hash = hashcode;
    wrapper->val = (void*)res;
    //set metatable
    return 1;
}

template <typename T>
int PushResult(lua_State * L, const T & res, bool needs_gc, bool is_const, int64_t hashcode, const char * mt_name)
{
    UserdataWrapper * wrapper = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
    wrapper->type_info.is_const = is_const;
    wrapper->type_info.needs_gc = needs_gc;
    wrapper->type_info.hash = hashcode;
    if (needs_gc)
    {
        wrapper->val = (void*)new T(res);
    }
    else
    {
        wrapper->val = (void*)(&res);
    }
    //set metatable
    return 1;
}

#endif   /* ----- #ifndef __CXXTOLUA_HELPER_HPP__  ----- */
