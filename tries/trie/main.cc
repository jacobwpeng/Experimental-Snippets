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
#include <boost/optional/optional.hpp>

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
        typedef boost::optional<ValueType> OptionalValueType;
        typedef typename boost::call_traits<ValueType>::param_type ValueParamType;

    private:
        struct TrieNode
        {
            char c;
            TrieNode** arr;
            ValueType v;
        };

        TrieNode * MakeTrieNode()               /* TODO:better performace */
        {
            TrieNode * node = new TrieNode;
            typedef TrieNode* TrieNodePtr;
            node->arr = new TrieNodePtr[alpha_.CharNum()];
            memset( node->arr, 0x0, alpha_.CharNum() );
            return node;
        }

    public:
        Trie( const Alphabet& alpha )
            :alpha_(alpha)
        {
            root_ = MakeTrieNode();
        }

        void Put(const string& key, ValueParamType v)
        {
            assert( root_ );
            TrieNode * p = root_;
            size_t idx = 0;
            while( idx != key.size() )
            {
                char c = key[idx];
                unsigned char_idx = alpha_.ToIndices(c);
                TrieNode * node = p->arr[char_idx];
                if( node == NULL )
                {
                    node = MakeTrieNode();
                    node->c = c;
                    p->arr[char_idx] = node;
                    /* no node for c */
                }
                p = node;
                ++idx;

                if( idx == key.size() )
                {
                    node->v = v;
                    /* break out */
                }
            }
        }

        OptionalValueType Get(const string& key)
        {
            assert( root_ );
            size_t idx = 0;
            TrieNode * p = root_;

            OptionalValueType res;

            while( idx != key.size() )
            {
                char c = key[idx];
                TrieNode * node = p->arr[ alpha_.ToIndices(c) ];
                if( node == NULL ) break;

                p = node;
                ++idx;

                if( idx == key.size() )
                {
                    res.reset(node->v);
                }
            }

            return res;
        }

    private:
        TrieNode * root_;
        Alphabet alpha_;
};

int main()
{
    srand( time(NULL) );
    Alphabet alpha("abcde1234567");
    Trie<int> t(alpha);

    t.Put("123abed", 123);

    Trie<int>::OptionalValueType val = t.Get("1234");
    assert( !val );

    val = t.Get("123abed");
    assert( val );
    assert( val.get() == 123 );
}
