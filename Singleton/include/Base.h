/*
 * =====================================================================================
 *
 *       Filename:  Base.h
 *        Created:  11/29/2013 02:37:46 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __BASE_H__
#define  __BASE_H__

#include "singleton.hpp"

class Base
{
    /*-----------------------------------------------------------------------------
     * 如果类中有MakeInstance函数会使用MakeInstance函数来进行实例的生成
     * 否则默认使用无参构造函数进行初始化
     *-----------------------------------------------------------------------------*/
    public:
        void DoSomething();
    private:
        static Base* MakeInstance() { return new Base(1); }
        Base(int i);
        int i_;
        ENABLE_SINGLETON(Base);
};

#endif   /* ----- #ifndef __BASE_H__  ----- */

