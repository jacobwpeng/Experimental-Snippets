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

#if 0
namespace A
{
    namespace B
    {
        LUA_EXPORT_STACK_OBJECT
        struct C {};
    }
}

namespace D
{
    namespace E
    {
        struct C {};

        LUA_EXPORT C & SayHello();
    }
}

#endif

namespace fx
{
    LUA_EXPORT_STACK_OBJECT
    LUA_EXPORT_HEAP_OBJECT
    class DynamicArray : boost::noncopyable
    {
        public:
            LUA_EXPORT DynamicArray();
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
            LUA_EXPORT int Size(void) const;
            LUA_EXPORT int Size(int val) const;
            LUA_EXPORT int Size(double d, int val = 3) const;
            LUA_EXPORT unsigned Size(vector<int> & vecs, vector<int> v = vector<int>(), std::string s = "12345") const;
            LUA_EXPORT const std::string * Size(const pair<int, int>& p, vector<int> * ptr, double d = 1.0);
#if 0
            LUA_EXPORT void Append(int element);
            LUA_EXPORT bool Empty() const;

            LUA_EXPORT int Get(const std::vector<int> * ptr, 
                    std::vector< pair<int, std::string> > & vecs) const;

            LUA_EXPORT std::map< std::vector< pair<int, std::string> >, std::string> & This(
                    const std::string ** ppstr,
                    std::string * pstr,
                    const std::string& str,
                    std::string s);

            LUA_EXPORT static std::string GetMessage();
            LUA_EXPORT static void PrintMessage(const char* str);

            LUA_EXPORT std::string SayHello() const;
#endif

        private:
            vector<int> v_;
    };
}
#endif   /* ----- #ifndef __DYNAMIC_ARRAY_H__  ----- */
