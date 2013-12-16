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

struct Reflection
{
    virtual ~Reflection(){}
};

struct ReflectionMgr
{
};

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
}

template<typename T, const char* Name>
const char* Reflection_tmpl<T, Name>::name()
{
    return Reflection_tmpl<T, Name>::TypeName;
}


#endif   /* ----- #ifndef __REFLECTION_H__  ----- */

