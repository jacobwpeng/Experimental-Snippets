/*
 * =====================================================================================
 *
 *       Filename:  reflection.h
 *        Created:  12/16/2013 04:48:22 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  Base Class for reflection
 *
 * =====================================================================================
 */


#ifndef  __REFLECTION_H__
#define  __REFLECTION_H__

#include <map>
#include <string>
using std::string;
using std::map;

struct Reflection
{
    virtual ~Reflection(){}
};

struct ReflectionMgr
{
    typedef map<string, Reflection*> class_map_t;
    static void Register(const string& name, Reflection* val);
    static Reflection* GetInstance(const string& name);
    static class_map_t* class_map;
};

ReflectionMgr::class_map_t* ReflectionMgr::class_map = NULL;

void ReflectionMgr::Register(const string& name, Reflection* val)
{
    if( ReflectionMgr::class_map == NULL )
        ReflectionMgr::class_map = new ReflectionMgr::class_map_t;
    ReflectionMgr::class_map->insert( ReflectionMgr::class_map_t::value_type(name, val) );
}

Reflection* ReflectionMgr::GetInstance(const string& name)
{
    ReflectionMgr::class_map_t::iterator iter = ReflectionMgr::class_map->find( name );
    if( iter == ReflectionMgr::class_map->end() ) return NULL;
    return iter->second;
}

template<typename T, const char* Name>
struct Reflection_tmpl : public Reflection
{
    Reflection_tmpl();
    virtual ~Reflection_tmpl() {}

    const char* name();

    static const char* TypeName;
};

template<typename T, const char* Name>
const char* Reflection_tmpl<T, Name>::TypeName = Name;

template<typename T, const char* Name>
Reflection_tmpl<T, Name>::Reflection_tmpl()
{
    ReflectionMgr::Register( Name, this );
}

template<typename T, const char* Name>
const char* Reflection_tmpl<T, Name>::name()
{
    return Reflection_tmpl<T, Name>::TypeName;
}


#endif   /* ----- #ifndef __REFLECTION_H__  ----- */

