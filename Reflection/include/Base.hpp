/*
 * =====================================================================================
 *
 *       Filename:  Base.h
 *        Created:  12/16/2013 02:27:57 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */


#ifndef  __BASE_H__
#define  __BASE_H__

#include "reflection.hpp"

namespace reflection
{
    struct BaseNames
    {
        static const char Base_Name[];
    };
    const char BaseNames::Base_Name[] = "Base";
}

class Base : public Reflection_tmpl<Base, reflection::BaseNames::Base_Name>
{
    public:
        Base(int i)
        :i_(i) {}
        virtual ~Base() {}

        int get() const { return this->i_; }
    private:
        int i_;
};

#endif   /* ----- #ifndef __BASE_H__  ----- */
