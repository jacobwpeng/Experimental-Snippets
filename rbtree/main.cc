/*
 * =====================================================================================
 *
 *       Filename:  test.cc
 *        Created:  10/05/14 11:46:18
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "rbtree.hpp"
#include <iostream>
#include <map>

int main(int argc, char ** argv)
{
    const int kMaxElement = 123;
    typedef RBTree<int, int> MapType;
    MapType t;
    for (int i = 0; i != kMaxElement; ++i)
    {
        auto res = t.insert(std::make_pair(kMaxElement - i, i));
        assert (res.second);
    }
    t[1] = 0;
    t[124] = 1;
    auto iter = t.begin();
    while (iter != t.end())
    {
        std::cout << iter->first << '\t' << iter->second << '\n';
        ++iter;
    }
}
