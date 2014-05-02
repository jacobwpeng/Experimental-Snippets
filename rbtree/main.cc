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
#include <boost/pool/object_pool.hpp>

using namespace std;

class RBTree
{
    public:
        typedef int KeyType;
        typedef unsigned ValueType;

    private:
        enum Color
        {
            kRed = 1,
            kBlack
        };

        typedef struct _RBNode
        {
            _RBNode()
                :c(kRed), p(NULL), l(NULL), r(NULL)
            {
            };

            KeyType k;
            ValueType v;
            Color c;
            _RBNode * p;
            _RBNode * l;
            _RBNode * r;
        } RBNode;

    public:
        RBTree()
            :root_(NULL)
        {
        }

        void Put(KeyType k, ValueType v)
        {
            root_ = Put(root_, k, v);
            root_->c = kBlack;
        }

    private:
        RBNode * Put(RBNode* node, KeyType k, ValueType v)
        {
            if( node == NULL )
            {
                RBNode * new_node = node_pool_.construct();
                new_node->k = k;
                new_node->v = v;
                return new_node;
            }
            else
            {
                if( k > node->k )
                {
                    node->r = Put(node->r, k, v);
                    node->r->p = KeepBalance(node);
                }
                else if( k < node->k )
                {
                    node->l = Put(node->l, k, v);
                    node->l->p = KeepBalance(node);
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
            if( node->r->c == kRed )
            {
                node = RotateLeft(node);
            }

            if( node->c == kRed )
            {
                node = RotateRight(node);
            }

            if( node->l->c == kRed and node->r->c == kRed )
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
            assert( node->l->c == kRed );
            assert( node->r );
            assert( node->r->c == kRed );

            node->c = kRed;
            node->l->c = kBlack;
            node->r->c = kBlack;
        }

    private:
        boost::object_pool<RBNode> node_pool_;
        RBNode * root_;
};

int main()
{
    RBTree t;
    t.Put(3, 5);
}
