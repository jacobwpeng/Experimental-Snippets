/*
 * =====================================================================================
 *
 *       Filename:  Base2.h
 *        Created:  11/29/2013 04:03:29 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */



#ifndef  __BASE2_H__
#define  __BASE2_H__

#include "singleton.hpp"

class Base2
{
    public:
        void DoSomething();
    private:
        static Base2* MakeInstance() { return new Base2(2); }
        Base2(int i);
        int i_;
        ENABLE_SINGLETON(Base2);
};

#endif   /* ----- #ifndef __BASE2_H__  ----- */

