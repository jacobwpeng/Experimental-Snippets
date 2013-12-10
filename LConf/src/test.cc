/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  11/21/2013 03:01:40 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  test app for lconf
 *
 * =====================================================================================
 */
#include <cassert>
#include <iostream>
#include "lconf.h"
#include "scalar_node.h"
using std::cout;

int main(int argc, char* argv[])
{
    if( argc != 3 )
    {
        cout << "Usage : " << argv[0] << " lua_file attr" << '\n';
        return -1;
    }
    LConf conf(argv[1]);
    if( conf.IsValid() )
    {
        LConf::ArrayRange range = conf.LookupTable("costs.goods.id");
        LConf::ArrayIterator iter = range.first;
        while( iter != range.second )
        {
            ScalarNode* p = dynamic_cast<ScalarNode*>(*iter);
            assert(p);
            if( p->name().empty() == false ) cout << "name = " << p->name() << " ";
            cout << "value = " << p->value() << '\n';
            ++iter;
        }

    }
    return 0;
}

