/*
 * =====================================================================================
 *       Filename:  main.cc
 *        Created:  14:45:25 Apr 18, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <boost/call_traits.hpp>

using namespace std;

class Alphabet
{
    public:
        explicit Alphabet(const string& chars)
        {
            for( size_t idx = 0; idx != chars.size(); ++idx )
            {
                indices_[chars[idx]] = idx;
            }
        }

        unsigned ToIndices(char c)
        {
            /* no check for not found */
            return indices_.find(c)->second;
        }

        unsigned CharNum() const { return indices_.size(); }

    private:
        map<char, unsigned> indices_;
};

template<typename _ValueType>
class Trie
{
    public:
        typedef _ValueType ValueType;
        typedef typename boost::call_traits<ValueType>::param_type ValueParamType;
    public:
        Trie( const Alphabet& alpha )
            :alpha_(alpha)
        {

        }

        void Put(const string& key, ValueParamType v)
        {

        }

    private:
        Alphabet alpha_;
};

int main()
{
    srand( time(NULL) );
}
