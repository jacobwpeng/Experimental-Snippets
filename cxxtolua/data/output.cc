#include "userdata_wrapper.h"
#include "dynamic_array.h"
#include <lua.hpp>
extern "C" {
    int fx_DynamicArray_Size(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 1)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180 )
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            if (not __check_failed)
            {
                int  __res =  __class_ptr->Size() ;
                lua_pushinteger(L, __res);
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_Empty(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 1)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180 )
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            if (not __check_failed)
            {
                bool  __res =  __class_ptr->Empty() ;
                lua_pushboolean(L, __res);
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_PrintMessage(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 1)
        {
            bool __check_failed = false;
            const char * str;
            if (lua_type(L, 1) == LUA_TSTRING)
            {
                str = luaL_checkstring(L, 1);
            }
            else if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == 756900392)
            {
                str = ((std::basic_string<char> *)__ud->val)->c_str();
            }
            else
            {
                __check_failed = true;
            }
            if (not __check_failed)
            {
                fx::DynamicArray::PrintMessage(str) ;
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_Get(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 2)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            int  index;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180 )
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            index = luaL_checkinteger(L, 2);
            if (not __check_failed)
            {
                int  __res =  __class_ptr->Get(index) ;
                lua_pushinteger(L, __res);
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_GetMessage(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 0)
        {
            bool __check_failed = false;
            if (not __check_failed)
            {
                const std::string __res = (fx::DynamicArray::GetMessage() );
                lua_pushlstring(L, __res.c_str(), __res.size());
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_Append(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 2)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            int  element;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180  && __ud->type_info.is_const == false)
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            element = luaL_checkinteger(L, 2);
            if (not __check_failed)
            {
                __class_ptr->Append(element) ;
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_TestOverload(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 2)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            const char * str;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180  && __ud->type_info.is_const == false)
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            if (lua_type(L, 2) == LUA_TSTRING)
            {
                str = luaL_checkstring(L, 2);
            }
            else if (lua_type(L, 2) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 2)) && __ud->type_info.hash == 756900392)
            {
                str = ((std::basic_string<char> *)__ud->val)->c_str();
            }
            else
            {
                __check_failed = true;
            }
            if (not __check_failed)
            {
                __class_ptr->TestOverload(str) ;
                return 1;
            }
        }
        else
        {
        }
        if (__top == 2)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            unsigned int  arg2;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180  && __ud->type_info.is_const == false)
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            arg2 = luaL_checkunsigned(L, 2);
            if (not __check_failed)
            {
                __class_ptr->TestOverload(arg2) ;
                return 1;
            }
        }
        else
        {
        }
        if (__top == 2)
        {
            bool __check_failed = false;
            fx::DynamicArray * __class_ptr;
            int  i;
            if (lua_type(L, 1) == LUA_TUSERDATA && (__ud = (UserdataWrapper*)lua_touserdata(L, 1)) && __ud->type_info.hash == -1683326180  && __ud->type_info.is_const == false)
            {
                __class_ptr = (fx::DynamicArray *)(__ud->val);
            }
            else
            {
                __check_failed = true;
            }
            i = luaL_checkinteger(L, 2);
            if (not __check_failed)
            {
                __class_ptr->TestOverload(i) ;
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_New(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 0)
        {
            bool __check_failed = false;
            if (not __check_failed)
            {
                UserdataWrapper * __res = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                __res->type_info.is_const = false;
                __res->type_info.needs_gc = false;
                __res->type_info.hash = -1683326180;
                __res->val = (void*) ( new fx::DynamicArray() );
                return 1;
            }
        }
        else
        {
        }
        if (__top == 1)
        {
            bool __check_failed = false;
            int  i;
            i = luaL_checkinteger(L, 1);
            if (not __check_failed)
            {
                UserdataWrapper * __res = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                __res->type_info.is_const = false;
                __res->type_info.needs_gc = false;
                __res->type_info.hash = -1683326180;
                __res->val = (void*) ( new fx::DynamicArray(i) );
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int fx_DynamicArray_DynamicArray(lua_State * L)
    {
        const int __top = lua_gettop(L);
        UserdataWrapper * __ud;
        if (__top == 0)
        {
            bool __check_failed = false;
            if (not __check_failed)
            {
                UserdataWrapper * __res = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                __res->type_info.is_const = false;
                __res->type_info.needs_gc = true;
                __res->type_info.hash = -1683326180;
                __res->val = (void*) ( new fx::DynamicArray() );
                return 1;
            }
        }
        else
        {
        }
        if (__top == 1)
        {
            bool __check_failed = false;
            int  i;
            i = luaL_checkinteger(L, 1);
            if (not __check_failed)
            {
                UserdataWrapper * __res = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                __res->type_info.is_const = false;
                __res->type_info.needs_gc = true;
                __res->type_info.hash = -1683326180;
                __res->val = (void*) ( new fx::DynamicArray(i) );
                return 1;
            }
        }
        else
        {
        }
        return luaL_error(L, "Cannot find proper function to call.");
    }

    int luaopen_array(lua_State * L)
    {
        static const struct luaL_Reg libfuncs[] = {
            {"New", fx_DynamicArray_New},
            {"DynamicArray", fx_DynamicArray_DynamicArray},
            {"GetMessage", fx_DynamicArray_GetMessage},
            {"PrintMessage", fx_DynamicArray_PrintMessage},
            {"TestOverload", fx_DynamicArray_TestOverload},
            {"Get", fx_DynamicArray_Get},
            {"Append", fx_DynamicArray_Append},
            {"Empty", fx_DynamicArray_Empty},
            {"Size", fx_DynamicArray_Size},
            {NULL, NULL}
        };
        luaL_newlib(L, libfuncs);
        return 1;
    }

}
