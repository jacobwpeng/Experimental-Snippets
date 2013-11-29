/*
 * =====================================================================================
 *
 *       Filename:  Base2.cc
 *        Created:  11/29/2013 04:05:27 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cstdio>
#include "Base.h"
#include "Base2.h"

SINGLETON_IMPL(Base2);

Base2::Base2(int i)
    :i_(i)
{
    printf("func : %s, this addr : 0x%x, i_ : %d\n", __PRETTY_FUNCTION__, (unsigned)this, this->i_);
}

void Base2::DoSomething()
{
    this->i_ += 10000;
    printf("func : %s, this addr : 0x%x, i_ : %d\n", __PRETTY_FUNCTION__, (unsigned)this, this->i_);
}


