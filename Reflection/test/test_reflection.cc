/*
 * =====================================================================================
 *
 *       Filename:  test_reflcpp.cc
 *        Created:  12/16/2013 03:49:49 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <gtest/gtest.h>
#include "Base.hpp"

TEST(reflection, CreateInstance)
{
    Base b(1024);
    Reflection* ref = ReflectionMgr::GetInstance("Base");
    Base* p = dynamic_cast<Base*>( ref );
    ASSERT_FALSE(NULL == p);

    ASSERT_EQ(1024, p->get() );
}
