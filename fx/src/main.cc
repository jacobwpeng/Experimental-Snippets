/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  15:54:03 Apr 02, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/typeof/typeof.hpp>
#include "map.hpp"

using std::cout;
using std::cerr;
using std::boolalpha;
using FX::Map;

namespace detail
{
    struct Base
    {
        ~Base()
        {
            cerr << "~Base" << '\n';
        }
        unsigned ival;
    };

    //template<typename T>
    //void Print(boost:call_traits<T>::param_type v)
    //{
    //    cout << v << '\t';
    //}
    //
    void Print(unsigned val)
    {
        cout << val << '\t';
    }
}

namespace FX
{
    template<>
    struct Converter<detail::Base>
    {
        Word_t operator()(const detail::Base& base)
        {
            return base.ival;
        }
    };
}

const size_t times = 1 << 4;

int TestJudyMap(int argc, char * argv[])
{
    typedef Map<unsigned, unsigned> U2UMap;

    U2UMap m;

    for( unsigned idx = 0; idx != times; ++idx )
    {
        m.insert( std::make_pair(idx, idx+1) );
    }
    cout << "size = " << m.size() << '\n';

    U2UMap::Iterator iter = m.find(5);
    m.erase( iter );

    cout << "size = " << m.size() << '\n';

    return 0;
}

int TestStdMap(int argc, char * argv[])
{
    std::map<unsigned, unsigned> m;
    for( unsigned idx = 0; idx != times; ++idx )
    {
        unsigned key = rand() % times;
        m.insert( std::make_pair(key, idx+1) );
    }
    cout << "size = " << m.size() << '\n';
    return 0;
}

int main(int argc, char * argv[])
{
    srand( time(NULL) );
    if( argc == 1 ) return TestJudyMap(argc, argv);
    else return TestStdMap(argc, argv);
}
