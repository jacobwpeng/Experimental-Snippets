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

#include <ctime>
#include <cstdlib>
#include <cassert>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/optional/optional.hpp>

using namespace std;

template<typename T>
class ObjectPool
{
    public:
        ObjectPool()
            :p_( sizeof(T) )
        { 
        }

        ~ObjectPool()
        { 
        }

        T * Construct()
        {
            return new (p_.malloc()) T;
        }

        void Destroy(T* ptr)
        {
            ptr->~T();
            p_.free(ptr);
        }

    private:
        boost::pool<> p_;
};

class RBTree
{
    public:
        typedef unsigned KeyType;
        typedef unsigned ValueType;
        typedef boost::optional<ValueType> OptionalValueType;

    private:
        enum Color
        {
            kRed = 0,
            kBlack
        };

        typedef struct _RBNode
        {
            _RBNode()
                :c(kRed), p(NULL), l(NULL), r(NULL)
            {
            };

            void FlipColor()
            {
                c = static_cast<Color>(1-c);
            }

            KeyType k;
            ValueType v;
            Color c;
            _RBNode * p;
            _RBNode * l;
            _RBNode * r;
        } RBNode;

    public:
        RBTree()
            :root_(NULL), height_(0)
        {
        }

        void Put(KeyType k, ValueType v)
        {
            root_ = Put(root_, k, v);
            if( root_->c == kRed )
            {
                ++height_;
                root_->c = kBlack;
            }
        }

        OptionalValueType Get(KeyType k)
        {
            return Get(root_, k);
        }

        ValueType GetMin() const
        {
            const RBNode * node = GetMin(root_);
            return node->v;
        }

        void DeleteMin()
        {
            root_ = DeleteMin(root_);
            if( root_ ) root_->c = kBlack;
        }

        size_t Height() const { return height_; }

    private:
        RBNode * Put(RBNode* node, KeyType k, ValueType v)
        {
            if( node == NULL )
            {
                RBNode * new_node = node_pool_.Construct();
                new_node->k = k;
                new_node->v = v;
                return new_node;
            }
            else
            {
                if( k > node->k )
                {
                    node->r = Put(node->r, k, v);
                    node = KeepBalance(node);
                }
                else if( k < node->k )
                {
                    node->l = Put(node->l, k, v);
                    node = KeepBalance(node);
                }
                else
                {
                    node->v = v;
                }
                return node;
            }
        }

        RBNode * KeepBalance( RBNode * node )
        {
            assert( node );
            if( node->r and node->r->c == kRed )
            {
                node = RotateLeft(node);
            }

            if( node->l and node->l->c == kRed and node->l->l and node->l->l->c == kRed )
            {
                node = RotateRight(node);
            }

            if( node->l and node->l->c == kRed and node->r and node->r->c == kRed )
            {
                FlipColors(node);
            }
            return node;
        }

        RBNode * RotateLeft( RBNode * node )
        {
            assert( node );
            RBNode * r = node->r;
            assert( r );
            assert( r->c == kRed );

            node->r = r->l;
            r->l = node;
            r->p = node->p;
            node->p = r;

            r->c = node->c;
            node->c = kRed;

            return r;
        }

        RBNode * RotateRight( RBNode * node )
        {
            assert( node );
            RBNode * l = node->l;
            assert( l );
            assert( l->c == kRed );

            node->l = l->r;
            l->r = node;
            l->p = node->p;
            node->p = l;

            l->c = node->c;
            node->c = kRed;

            return l;
        }

        void FlipColors( RBNode * node )
        {
            assert( node );
            assert( node->l );
            assert( node->r );

            node->FlipColor();
            node->l->FlipColor();
            node->r->FlipColor();
        }

        OptionalValueType Get(RBNode * node, KeyType k)
        {
            if( node == NULL ) { return OptionalValueType(); }
            if( k == node->k ) return node->v;
            else if( k > node->k ) return Get(node->r, k);
            else return Get(node->l, k);
        }

        const RBNode * GetMin(RBNode * node) const
        {
            assert( node );
            while( node->l != NULL )
            {
                node = node->l;
            }

            return node;
        }

        RBNode * DeleteMin(RBNode * node)
        {
            assert( node );
            if( node->l == NULL )
            {
                node_pool_.Destroy( node );
                return NULL;
            }

            if( node->l->c == kBlack and node->l->l and node->l->l->c == kBlack )
            {
                node = MoveRedLeft( node );
            }
            node->l = DeleteMin( node->l );
            return node;
        }

        RBNode * MoveRedLeft( RBNode * node )
        {
            assert( node );
            assert( node->l );
            assert( node->l->c == kBlack );
            assert( node->l->l );
            assert( node->l->l->c == kBlack );
            FlipColors(node);
            return NULL;
        }

    private:
        ObjectPool<RBNode> node_pool_;
        RBNode * root_;
        size_t height_;
};

int main(int argc, char * argv[])
{
    srand( time(NULL) );
    (void)argv;

    const size_t kNodeCount = 1 << 20;
    RBTree t;
    map<unsigned, unsigned> m;
    set<unsigned> keys;

    for( size_t idx = 0; idx != kNodeCount; ++idx )
    {
        unsigned k = rand();
        //keys.insert(k);
        t.Put( k, idx );
        m[k] = idx;
    }

    cout << t.Height() << '\n';

    //BOOST_FOREACH( unsigned k, keys )
    //{
    //    RBTree::OptionalValueType opt_val = t.Get(k);
    //    assert( opt_val );
    //    assert( opt_val.get() == m[k] );
    //}

    //assert( t.GetMin() == m.begin()->second );

    //if( argc == 1 )
    //{
    //    cout << "Test RBTree\n";
    //    for( size_t idx = 0; idx != kNodeCount; ++idx )
    //        t.Put( idx, idx );
    //}
    //else
    //{
    //    cout << "Test std::map\n";
    //    for( size_t idx = 0; idx != kNodeCount; ++idx )
    //        m[idx] = idx;
    //}
    //cout << "Wait for input...\n";
    //char c;
    //cin >> c;
}
