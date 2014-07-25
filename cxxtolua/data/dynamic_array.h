/*
 * =====================================================================================
 *
 *       Filename:  dynamic_array.h
 *        Created:  06/26/14 15:03:21
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __DYNAMIC_ARRAY_H__
#define  __DYNAMIC_ARRAY_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <string>
#include <boost/noncopyable.hpp>

#define LUA_EXPORT
#define LUA_EXPORT_STACK_OBJECT // A();
#define LUA_EXPORT_HEAP_OBJECT // new A(); delete ptr;
using std::map;
using std::vector;
using std::pair;

namespace fx
{
    LUA_EXPORT_STACK_OBJECT
    LUA_EXPORT_HEAP_OBJECT
    class DynamicArray
    {
        public:
            LUA_EXPORT DynamicArray();
            LUA_EXPORT DynamicArray(int i);
#if 0
            construct on heap -> static DynamicArray* New(arg1, arg2, ....);
            construct on stack -> static DynamicArray* DynamicArray(arg1, arg2, ...);

            destructor -> static void Delete(DynamicArray* array);
            int fx_DynamicArray_DynamicArray(lua_State * L)
            {
                UserdataWrapper * wrapper = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                wrapper->type_info.is_const = false;
                wrapper->type_info.needs_gc = true;
                wrapper->type_info.hash = hash('fx::DynamicArray');
                wrapper->val = (void*)(new fx::DynamicArray);
                return 1;
            }

            int fx_DynamicArray_New(lua_State* L)
            {
                UserdataWrapper * wrapper = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                wrapper->type_info.is_const = false;
                wrapper->type_info.needs_gc = false;
                wrapper->type_info.hash = hash('fx::DynamicArray');
                wrapper->val = (void*)(new fx::DynamicArray);

                return 1;
            }
            LUA_EXPORT DynamicArray(int element, vector<pair<int, double> >& vecs);
            int fx_DynamicArray_New(lua_State* L)
            {
                UserdataWrapper * wrapper = (UserdataWrapper*) lua_newuserdata(L, sizeof(UserdataWrapper));
                wrapper->type_info.is_const = false;
                wrapper->type_info.needs_gc = false;
                wrapper->type_info.hash = hash('fx::DynamicArray');
                wrapper->val = (void*)(new fx::DynamicArray);

                return 1;
            }
#endif
            LUA_EXPORT int Size() const;
            LUA_EXPORT void Append(int element);
            LUA_EXPORT bool Empty() const;
            LUA_EXPORT int Get(int index) const;

            LUA_EXPORT static std::string GetMessage();
            LUA_EXPORT static void PrintMessage(const char* str);

            LUA_EXPORT static DynamicArray * New();
            LUA_EXPORT static void Delete(DynamicArray * ptr);

            LUA_EXPORT void TestOverload(int i, std::string str = "default value");
            LUA_EXPORT void TestOverload(std::string str);
            LUA_EXPORT void TestOverload();

        private:
            vector<int> v_;
    };
}
#endif   /* ----- #ifndef __DYNAMIC_ARRAY_H__  ----- */
