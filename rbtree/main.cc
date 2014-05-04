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
            :root_(NULL), size_(0)
        {
        }

        void Put(KeyType k, ValueType v)
        {
            root_ = Put(root_, k, v);
            if( root_->c == kRed )
            {
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

        size_t Delete( KeyType k )
        {
            try
            {
                root_ = Delete( root_, k );
                return 1;
            }catch(...)
            {
                return 0;
            }
        }

        size_t size() const { return size_; }

    private:
        RBNode * Put(RBNode* node, KeyType k, ValueType v)
        {
            if( node == NULL )
            {
                RBNode * new_node = node_pool_.Construct();
                new_node->k = k;
                new_node->v = v;
                ++size_;
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

        RBNode * GetMin(RBNode * node) const
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
                --size_;
                return NULL;
            }

            if( node->l->c == kBlack and (node->l->l == NULL or node->l->l->c == kBlack) )
            {
                node = MoveRedLeft( node );
            }
            node->l = DeleteMin( node->l );
            return KeepBalance(node);
        }

        RBNode * MoveRedLeft( RBNode * node )
        {
            assert( node );
            assert( node->l );
            assert( node->l->c == kBlack );
            assert( node->l->l == NULL or node->l->l->c == kBlack );
            FlipColors(node);

            if( node->r->l and node->r->l->c == kRed )
            {
                node->r = RotateRight( node->r );
                node = RotateLeft( node );
                FlipColors(node);
            }
            return node;
        }

        RBNode * MoveRedRight( RBNode * node )
        {
            assert( node );
            assert( node->r );
            assert( node->r->c == kBlack );
            assert( node->r->r == NULL or node->r->r->c == kBlack );

            FlipColors(node);
            if( node->l->l and node->l->l->c == kRed )
            {
                node = RotateRight(node);
                FlipColors(node);
            }

            return node;
        }

        RBNode * Delete( RBNode * node, KeyType k )
        {
            assert( node );

            if( k < node->k )
            {
                if( node->l == NULL ) throw 0;
                if( node->l->c == kBlack and ( node->l->l == NULL or node->l->l->c == kBlack ) )
                {
                    node = MoveRedLeft( node );
                }

                node->l = Delete( node->l, k );
            }
            else
            {
                if( node->l != NULL and node->l->c == kRed )
                {
                    node = RotateRight( node );
                }

                if( k == node->k and node->r == NULL )
                {
                    node_pool_.Destroy(node);
                    --size_;
                    return NULL;
                }

                if( node->r->c == kBlack and (node->r->r == NULL or node->r->r->c == kBlack))
                {
                    node = MoveRedRight(node);
                }

                if( k == node->k )
                {
                    RBNode * successor = GetMin(node->r);
                    swap( successor->p, node->p );
                    swap( successor->l, node->l );
                    swap( successor->r, node->r );
                    swap( successor->c, node->c );

                    node = successor;

                    node->r = DeleteMin( node->r );
                }
                else
                {
                    if( node->r == NULL ) throw 0;
                    node->r = Delete( node->r, k );
                }
            }
            return KeepBalance(node);
        }

    private:
        ObjectPool<RBNode> node_pool_;
        RBNode * root_;
        size_t size_;
};

int main(int argc, char * argv[])
{
    srand( time(NULL) );
    (void)argv;

    const size_t kNodeCount = 1 << 20;
    RBTree t;
    map<unsigned, unsigned> m;
    typedef map<unsigned, unsigned>::value_type value_type;
    set<unsigned> keys;

    for( size_t idx = 0; idx != kNodeCount; ++idx )
    {
        unsigned k = rand() % kNodeCount;
        t.Put( k, idx );
        m[k] = idx;
    }

    assert( t.GetMin() == m.begin()->second );

    cout << "size = " << t.size() << '\n';

    BOOST_FOREACH( const value_type& p, m )
        assert( t.Delete( p.first ) == 1 );

    cout << "size = " << t.size() << '\n';

}
