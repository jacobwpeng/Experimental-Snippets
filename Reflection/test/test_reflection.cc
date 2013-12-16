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
    Base b;
    ASSERT_STREQ("Base", b.name());
    /*
     * Reflection* ref = ReflectionMgr::CreateInstance("Base");
     * Base* p = dynamic_cast<Base*>( ref );
     * ASSERT_NE(NULL, p);
     */
}
