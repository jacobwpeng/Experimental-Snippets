/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  08/15/14 15:18:09
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * =====================================================================================
 */

#include "dynamic_array.h"
#include "cxxtolua_helper.hpp"

int Func(lua_State * L)
{
    const std::vector<int, std::allocator<int> > * vec;
    int val;
    if (CheckArgument(L, 1, &vec, 2473338958754726831)
            && CheckArgument(L, 2, &val)
       )
    {
        fx::SayHello(*vec, val);
        return 0;
    }
}

int main()
{
    Func(NULL);
}
