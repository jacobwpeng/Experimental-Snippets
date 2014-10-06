/*
 * =====================================================================================
 *
 *       Filename:  rbtree.hpp
 *        Created:  10/05/14 11:36:46
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __RBTREE_HPP__
#define  __RBTREE_HPP__

#ifndef NDEBUG
#include <string>
#include <iostream>
#endif

#include <iterator>
#include <cassert>
#include <utility>
#include <functional>

template<typename _Key, typename _Value, typename _Comparator = std::less<_Key>>
class RBTree
{
    public:
        typedef _Key key_type;
        typedef _Value mapped_type;
        typedef std::pair<const key_type, mapped_type> value_type;
        typedef _Comparator key_compare;
        class iterator;
        class const_iterator;

    private:
        enum class RBNodeColor
        {
            kRed = 0,
            kBlack = 1,
        };

        struct RBNode
        {
            RBNode(const value_type& p)
                :v(p), l(NULL), r(NULL), c(RBNodeColor::kRed)
            { 
            }
            value_type v;
            RBNode * p;
            RBNode * l;
            RBNode * r;
            RBNodeColor c;
        };

        bool is_black(const RBNode * node) const
        {
            return node == NULL or node->c == RBNodeColor::kBlack;
        }

        bool is_red(const RBNode * node) const
        {
            return node != NULL and node->c == RBNodeColor::kRed;
        }

        int num_of_children(const RBNode * node) const
        {
            return (node->l ? 1 : 0) + (node->r ? 1: 0);
        }

        RBNode * predecessor(const RBNode * node) const
        {
            assert (node);
            RBNode * res = NULL;

            if (node->l)
            {
                res = node->l;
                while (res->r) res = res->r;
            }
            else
            {
                res = node->p;
                while (res and res->l == node)
                {
                    node = res;
                    res = node->p;
                }
            }
            return res;
        }
        
        RBNode * successor(const RBNode * node) const
        {
            assert (node);
            RBNode * res = NULL;
            if (node->r)
            {
                res = node->r;
                while (res->l) res = res->l;
            }
            else
            {
                res = node->p;
                while (res and res->r == node)
                {
                    node = res;
                    res = node->p;
                }
            }
            return res;
        }

        RBNode * min() const
        {
            RBNode * res = root_;
            while (res and res->l) res = res->l;
            return res;
        }

        RBNode * max() const
        {
            RBNode * res = root_;
            while (res and res->r) res = res->r;
            return res;
        }


        void unlink_leaf(RBNode * node) const
        {
            assert (node);
            if (root_ == node) return;
            assert (node->p);
            assert (node->p->l == node or node->p->r == node);
            if (node->p->l == node) node->p->l = NULL;
            else node->p->r = NULL;
        }

        RBNode * new_node(const value_type& p)
        {
            return new RBNode(p);
        }

        void destroy_node(RBNode * node)
        {
            delete node;
        }

        void recursive_destroy_node(RBNode * node)
        {
            if (node == NULL) return;
            if (node->l) recursive_destroy_node(node->l);
            if (node->r) recursive_destroy_node(node->r);
            destroy_node(node);
        }

        void rotate_left(RBNode * node)
        {
            //std::cout << "rotate left " << node->v.first << '\n';
            assert (node);
            RBNode * r = node->r;
            assert (r);
            if (node->p)
            {
                if (node->p->l == node) node->p->l = r;
                else node->p->r = r;
            }
            else
            {
                root_ = r;
            }
            r->p = node->p;
            node->p = r;
            if (r->l)
            {
                r->l->p = node;
            }
            node->r = r->l;
            r->l = node;
        }

        void rotate_right(RBNode * node)
        {
            //std::cout << "rotate right " << node->v.first << '\n';
            assert (node);
            RBNode * l = node->l;
            assert (l);
            if (node->p)
            {
                if (node->p->l == node) node->p->l = l;
                else node->p->r = l;
            }
            else
            {
                root_ = l;
            }
            l->p = node->p;
            node->p = l;
            if (l->r)
            {
                l->r->p = node;
            }
            node->l = l->r;
            l->r = node;
        }

        //void swap_position(RBNode * lhs, RBNode * rhs)
        //{
        //    assert (lhs);
        //    assert (rhs);
        //}

        void insert_fixup(RBNode * node)
        {
            assert(node);
            while (node != root_ and is_red(node->p))
            {
                RBNode * p = node->p;
                RBNode * ps = (p->p->l == p) ? p->p->r : p->p->l; /* p is red, then p->p != NULL */
                if (is_red(ps))
                {
                    assert (p->p->c == RBNodeColor::kBlack);
                    p->p->c = RBNodeColor::kRed;
                    p->c = RBNodeColor::kBlack;
                    ps->c = RBNodeColor::kBlack;
                    node = p->p;
                }
                else
                {
                    RBNode * pp = p->p;
                    assert (pp);
                    if (pp->l == p)
                    {
                        if (p->r == node)
                        {
                            p->r->c = p->c;
                            p->c = RBNodeColor::kRed;
                            rotate_left(p);
                            std::swap(node, p);
                        }
                        p->c = pp->c;
                        pp->c = RBNodeColor::kRed;
                        rotate_right(pp);
                    }
                    else
                    {
                        assert (pp->r == p);
                        if (p->l == node)
                        {
                            p->l->c = p->c;
                            p->c = RBNodeColor::kRed;
                            rotate_right(p);
                            std::swap(node, p);
                        }
                        p->c = pp->c;
                        pp->c = RBNodeColor::kRed;
                        rotate_left(pp);
                    }
                    break;
                }
            }
            root_->c = RBNodeColor::kBlack;
        }

        iterator insert_impl(RBNode * prev, int res, const value_type& p)
        {
            RBNode * node = new_node(p);
            node->p = prev;
            if (prev != NULL)
            {
                assert (res != 0);
                if (res < 0) { prev->l = node; }
                else { prev->r = node; }
            }
            else
            {
                node->c = RBNodeColor::kBlack;
                root_ = node;
            }
            if (is_red(node->p))
            {
                insert_fixup(node);
            }
            return iterator(this, node);
        }

        void erase_fixup(RBNode * node)
        {
            assert (node);
            while (node != root_ and is_black(node))
            {
                RBNode * p = node->p;
                assert (p);
                RBNode * s = p->l == node ? p->r : p->l;
                assert (s);
                if (is_red(s))
                {
                    assert (is_black(p));
                    assert (s->l);
                    assert (s->r);
                    assert (is_black(s->l));
                    assert (is_black(s->r));
                    p->c = RBNodeColor::kRed;
                    s->c = RBNodeColor::kBlack;
                    if (s == p->r) rotate_left(p);
                    else rotate_right(p);
                }
                else
                {
                    /* sibling is black */
                    if (is_black(s->l) and is_black(s->r))
                    {
                        s->c = RBNodeColor::kRed;
                        if (is_red(p))
                        {
                            p->c = RBNodeColor::kBlack;
                            break;
                        }
                        else
                        {
                            node = p;
                            continue;
                        }
                    }

                    if (node == p->l)
                    {
                        if (is_red(s->l))
                        {
                            s->l->c = RBNodeColor::kBlack;
                            s->c = RBNodeColor::kRed;
                            rotate_right(s);
                            s = s->p;
                        }

                        assert (is_black(s->l));
                        assert (is_red(s->r));
                        s->c = p->c;
                        s->r->c = RBNodeColor::kBlack;
                        p->c = RBNodeColor::kBlack;
                        rotate_left(p);
                        node = root_;
                    }
                    else
                    {
                        if (is_red(s->r))
                        {
                            s->r->c = RBNodeColor::kBlack;
                            s->c = RBNodeColor::kRed;
                            rotate_left(s);
                            s = s->p;
                        }

                        assert (is_black(s->r));
                        assert (is_red(s->l));
                        s->c = p->c;
                        s->l->c = RBNodeColor::kBlack;
                        p->c = RBNodeColor::kBlack;
                        rotate_right(p);
                        node = root_;
                    }
                }
            }
            assert (root_);
            root_->c = RBNodeColor::kBlack;
        }

        void erase_impl(RBNode * node)
        {
            assert (node);
            RBNode * node_to_del;
            int children_count = num_of_children(node);
            if (children_count == 0 and is_red(node))
            {
                /* red leaf node */
                node_to_del = node;
            }
            else if (node == root_ and children_count == 0)
            {
                /* root only */
                root_ = NULL;
                destroy_node(node);
                return;
            }
            else if (is_black(node) and children_count == 1)
            {
                /* black node with single child */
                /* the child must be single red leaf node */
                node_to_del = node;
            }
            else if (is_black(node) and children_count == 0)
            {
                /* black leaf node, cannot be root_ */
                node_to_del = node;
            }
            else
            {
                /* internal node */
                /* all internal node must have successor node */
                assert (children_count == 2);
                RBNode * successor = this->successor(node);
                //std::cout << "successor " << successor->v.first << '\n';
                /* swap position */
                if (node->r == successor)
                {
                    if (node->p)
                    {
                        if (node->p->l == node) node->p->l = successor;
                        else node->p->r = successor;
                    }
                    assert (successor->l == NULL);
                    if (successor->r) successor->r->p = node;

                    node->r = successor->r;
                    successor->r = node;
                    successor->p = node->p;
                    node->p = successor;
                    node->l->p = successor;
                    std::swap(successor->l, node->l);
                    node_to_del = node;
                }
                else
                {
                    assert (successor->l == NULL);
                    assert (successor->p);
                    assert (successor == successor->p->l);
                    if (node->p)
                    {
                        if (node->p->l == node) node->p->l = successor;
                        else node->p->r = successor;
                    }
                    else
                    {
                        root_ = successor;
                    }
                    node->l->p = successor;
                    node->r->p = successor;
                    successor->p->l = node;
                    std::swap(successor->l, node->l);
                    std::swap(successor->p, node->p);
                    assert (node->r);
                    if (successor->r) successor->r->p = node;
                    std::swap(successor->r, node->r);
                    node_to_del = node;
                }
            }
            children_count = num_of_children(node_to_del);
            if (is_red(node_to_del) and children_count == 0)
            {
                unlink_leaf(node_to_del);
            }
            else if (is_black(node_to_del) and children_count == 1)
            {
                RBNode * child = node_to_del->l ? node_to_del->l : node_to_del->r;
                assert (is_red(child));
                assert (num_of_children(child) == 0);
                child->p = node_to_del->p;
                if (node_to_del->p)
                {
                    if (node_to_del->p->l == node_to_del) node_to_del->p->l = child;
                    else node_to_del->p->r = child;
                }
                else
                {
                    /* delete root_ */
                    root_ = child;
                }
                child->c = RBNodeColor::kBlack;
            }
            else
            {
                /* black leaf node only */
                erase_fixup(node_to_del);
                /* adjust real_node parent */
                assert (node_to_del->p);
                if (node_to_del->p->l == node_to_del) node_to_del->p->l = NULL;
                else node_to_del->p->r = NULL;
            }
            destroy_node(node_to_del);
        }

    public:
        template<typename Derived, typename Tree, typename Node, typename Pointer, typename Reference>
        class iterator_base
        {
            public:
                Derived & operator++ ()
                {
                    assert (tree_);
                    assert (node_);
                    node_ = tree_->successor(node_);
                    return static_cast<Derived&>(*this);
                }

                const Derived operator++ (int)
                {
                    auto res = static_cast<Derived&>(*this);
                    ++(*this);
                    return res;
                }

                Derived & operator-- ()
                {
                    assert (tree_);
                    assert (node_);
                    node_ = tree_->predecessor(node_);
                    return static_cast<Derived&>(*this);
                }

                const Derived operator-- (int)
                {
                    auto res = static_cast<Derived&>(*this);
                    --(*this);
                    return res;
                }

                Reference operator* () const
                {
                    assert (tree_);
                    assert (node_);
                    return node_->v;
                }

                Pointer operator-> () const
                {
                    return &**this;
                }

                template<typename Derived2, typename Tree2, typename Node2, typename Pointer2, typename Reference2>
                bool operator== (const iterator_base<Derived2, Tree2, Node2, Pointer2, Reference2>& rhs)
                {
                    return tree_ == rhs.tree_ and node_ == rhs.node_;
                }

                template<typename Derived2, typename Tree2, typename Node2, typename Pointer2, typename Reference2>
                bool operator!= (const iterator_base<Derived2, Tree2, Node2, Pointer2, Reference2>& rhs)
                {
                    return !(*this == rhs);
                }
            protected:
                template<typename Derived2, typename Tree2, typename Node2, typename Pointer2, typename Reference2>
                friend class iterator_base;
                friend class RBTree;

                iterator_base(Tree * tree = NULL, Node * node = NULL)
                    :tree_(tree), node_(node)
                {
                }

            private:
                Tree * tree_;
                Node * node_;
        };

        class iterator : public std::iterator<std::bidirectional_iterator_tag, value_type>
                         , public iterator_base<iterator, RBTree, RBNode, value_type*, value_type&>
        {
            public:
                iterator() {}
                iterator(RBTree * tree)
                    :iterator_base<iterator, RBTree, RBNode, value_type*, value_type&>(tree) {}

            private:
                iterator(RBTree * tree, RBNode * node)
                    :iterator_base<iterator, RBTree, RBNode, value_type*, value_type&>(tree, node) {}
                friend class RBTree;
                friend class const_iterator;
        };

        class const_iterator : public std::iterator<std::bidirectional_iterator_tag, value_type>
                               , public iterator_base<const_iterator, const RBTree, const RBNode, const value_type*, const value_type&>
        {
            public:
                const_iterator(){}
                const_iterator(const RBTree * tree)
                    :iterator_base<const_iterator, const RBTree, const RBNode, const value_type*, const value_type&>(tree) {}
                const_iterator(const iterator& rhs)
                    :iterator_base<const_iterator, const RBTree, const RBNode, const value_type*, const value_type&>(rhs.tree_, rhs.node_)
                    {}
            private:
                friend class RBTree;
        };

    public:
        RBTree()
            :root_(NULL), size_(0)
        {
        }

        ~RBTree()
        {
            recursive_destroy_node(root_);
        }

        iterator begin() { return iterator(this, min()); }
        iterator end() { return iterator(this); }
        const_iterator begin() const { return const_iterator(this, min()); }
        const_iterator end() const { return const_iterator(this); }

        iterator find(key_type key)
        {
            RBNode * node = root_;
            while (node != NULL)
            {
                bool less = comp_(key, node->v.first);
                bool greater = comp_(node->v.first, key);
                if (not less and not greater) return iterator(this, node);
                else if (less) node = node->l;
                else node = node->r;
            }
            return end();
        }

        size_t erase(key_type key)
        {
            auto res = find(key);
            if (res == end()) return 0;
            else 
            {
                erase(res);
                return 1;
            }
        }

        iterator erase(iterator iter)
        {
            assert (iter.tree_ == this);
            assert (iter != end());
            std::cout << "erase " << iter.node_->v.first << '\n';
            RBNode * successor = this->successor(iter.node_);
            erase_impl(iter.node_);
            assert (size_ >= 1);
            --size_;
            return iterator(this, successor);
        }

        std::pair<iterator, bool> insert(const value_type& p)
        {
            //std::cout << "insert " << p.first << '\n';
            RBNode * prev = root_;
            RBNode * node = root_;
            int res = 0;
            while (node != NULL)
            {
                bool less = comp_(p.first, node->v.first);
                bool greater = comp_(node->v.first, p.first);
                if (less) res = -1;
                else if (greater) res = 1;
                else res = 0;
                if (res == 0) break;
                else
                {
                    prev = node;
                    if (res < 0) node = node->l;
                    else node = node->r;
                }
            }
            if (node == prev)
            {
                assert (prev == NULL);
                /* root */
                ++size_;
                return std::make_pair(insert_impl(prev, res, p), true);
            }
            else if (res == 0)
            {
                node->v.second = p.second;
                return std::make_pair(iterator(this, node), false);
            }
            else
            {
                ++size_;
                return std::make_pair(insert_impl(prev, res, p), true);
            }
        }

        mapped_type & operator[] (key_type key)
        {
            auto res = insert(std::make_pair(key, mapped_type()));
            return res.first->second;
        }

        void swap(RBTree & rhs)
        {
            std::swap(root_, rhs.root_);
            std::swap(size_, rhs.size_);
        }

        size_t size() const { return size_; }
        bool empty() const { return size_ == 0; }
        size_t count(key_type key) const { return find(key) == end() ? 0 : 1; }

#ifndef NDEBUG
        void preorder_print_impl(const std::string& prefix, RBNode * node)
        {
            if (node == NULL) return;
            std::cout << prefix << node->v.first << ' ' << (node->c == RBNodeColor::kRed ? "red" : "black") << '\n';
            preorder_print_impl(prefix + "──", node->l);
            preorder_print_impl(prefix + "──", node->r);
        }

        void preorder_print()
        {
            if (root_) std::cout << "root_ " << root_->v.first << '\n';
            preorder_print_impl("├──", root_);
        }
#endif

    private:
        friend class iterator;
        friend class const_iterator;
        RBNode * root_;
        size_t size_;
        key_compare comp_;
};
#endif   /* ----- #ifndef __RBTREE_HPP__  ----- */
