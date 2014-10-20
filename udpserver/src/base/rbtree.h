/*
 * =====================================================================================
 *
 *       Filename:  rbtree.h
 *        Created:  08/23/14 11:05:43
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __RBTREE_H__
#define  __RBTREE_H__

#include <cassert>
#include <cstring>
#include <memory>
#include <vector>
#include <iostream>
#include <type_traits>
#include "memory_list.h"

namespace fx
{
    namespace base
    {
        namespace container
        {
            template <typename _Key, typename _Value, typename _Comparator = std::less<_Key>, typename KeyEnable = void, typename ValueEnable = void>
            class RBTree;

            template <typename _Key, typename _Value, typename _Comparator>
            class RBTree<_Key, _Value, _Comparator, 
                  typename std::enable_if <std::is_pod<_Key>::value >::type , typename std::enable_if< std::is_pod<_Value>::value >::type>
            {
                public:
                    typedef RBTree<_Key, _Value, _Comparator, 
                            typename std::enable_if <std::is_pod<_Key>::value >::type, 
                            typename std::enable_if< std::is_pod<_Value>::value >::type> ThisType;
                    typedef _Key key_type;
                    typedef _Value mapped_type;
                    typedef _Comparator key_compare;

                    typedef struct { key_type first; mapped_type second; } value_type;

                private:
                    typedef fx::base::MemoryList::SliceId NodeId;
                    static const NodeId kInvalidNodeId = fx::base::MemoryList::kInvalidSliceId;
                    static const int64_t kMagicNumber = 1050165255719967046;
                    struct MetaData
                    {
                        int64_t magic_number;
                        NodeId root;
                        size_t size;
                        size_t depth;
                    };

                    enum Color
                    {
                        kRed = 0,
                        kBlack = 1
                    };

                    struct RBNode
                    {
                        Color c;
                        NodeId p;               /* parent */
                        NodeId l;               /* left */
                        NodeId r;               /* right */
                        value_type v;

                        void Init(key_type key, mapped_type val)
                        {
                            c = kRed;
                            p = l = r = kInvalidNodeId;
                            v.first = key;
                            v.second = val;
                        }

                        void FlipColor()
                        {
                            c = (c == kRed ? kBlack : kRed);
                        }
                    };

                    template<typename DerivedType, typename ContainerType, typename Pointer, typename Reference>
                    class IteratorBase
                    {
                        public:
                            DerivedType& operator++()
                            {
                                assert (tree_);
                                assert (id_ != kInvalidNodeId);
                                id_ = tree_->GetNext(id_);
                                return static_cast<DerivedType&>(*this);
                            }

                            const DerivedType operator++(int)
                            {
                                assert (tree_);
                                assert (id_ != kInvalidNodeId);
                                auto result = static_cast<DerivedType&>(*this);
                                ++*this;
                                return result;
                            }

                            template<typename DerivedType2, typename ContainerType2, typename Pointer2, typename Reference2>
                            bool operator == (const IteratorBase<DerivedType2, ContainerType2, Pointer2, Reference2> & rhs) const
                            {
                                return tree_ == rhs.tree_ and id_ == rhs.id_;
                            }

                            template<typename DerivedType2, typename ContainerType2, typename Pointer2, typename Reference2>
                            bool operator != (const IteratorBase<DerivedType2, ContainerType2, Pointer2, Reference2> & rhs) const
                            {
                                return !(*this == rhs);
                            }

                            Reference operator* () const { return tree_->GetPointer(id_)->v; }
                            Pointer operator-> () const { return &**this; }

                        protected:
                            template<typename DerivedType2, typename ContainerType2, typename Pointer2, typename Reference2>
                            friend class IteratorBase;

                            IteratorBase(ContainerType * tree = NULL, NodeId id = kInvalidNodeId)
                                :tree_(tree), id_(id)
                            {
                            }

                        protected:
                            ContainerType * tree_;
                            NodeId id_;
                    };
                    template<typename DerivedType, typename ContainerType, typename Pointer, typename Reference>
                    friend class IteratorBase;

                public:
                    static const size_t kHeaderSize = sizeof(MetaData);
                    class iterator;
                    friend class iterator;

                    class const_iterator : public std::iterator<std::forward_iterator_tag, value_type>, 
                                           public IteratorBase<const_iterator, const ThisType, const value_type*, const value_type&>
                    {
                        public:
                            const_iterator() {}

                            const_iterator(iterator iter)
                                :IteratorBase<const_iterator, const ThisType, const value_type*, const value_type&>(iter.tree_, iter.id_){}
                            const_iterator(const ThisType * tree, NodeId id)
                                :IteratorBase<const_iterator, const ThisType, const value_type*, const value_type&>(tree, id) { } 
                            friend class RBTree;
                    };
                    friend class const_iterator;

                    class iterator : public std::iterator<std::forward_iterator_tag, value_type>, 
                                     public IteratorBase<iterator, ThisType, value_type*, value_type&>
                    {
                        public:
                            iterator() {}

                        private:
                            iterator(ThisType * tree, NodeId id) :IteratorBase<iterator, ThisType, value_type*, value_type&>(tree, id) { } 
                            friend class RBTree;
                            friend class const_iterator;
                    };

                private:
                    RBNode * GetPointer(NodeId id)
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<RBNode*>(ml_->GetBuffer(id));
                    }

                    const RBNode * GetPointer(NodeId id) const
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<const RBNode*>(ml_->GetBuffer(id));
                    }

                    NodeId KeepBalance(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * right = GetPointer(node->r);
                        if (right and right->c == kRed)
                        {
                            node_id = RotateLeft(node_id);
                            node = GetPointer(node_id);
                            assert (node);
                        }

                        RBNode * left = GetPointer(node->l);
                        if (left and left->c == kRed)
                        {
                            RBNode * lol = GetPointer(left->l);
                            if (lol and lol->c == kRed)
                            {
                                node_id = RotateRight(node_id);
                                node = GetPointer(node_id);
                                assert (node);
                            }
                        }

                        right = GetPointer(node->r);
                        left = GetPointer(node->l);

                        if (left and left->c == kRed and right and right->c == kRed)
                        {
                            FlipColors(node_id);
                        }

                        return node_id;
                    }

                    NodeId RotateLeft(NodeId id)
                    {
                        RBNode * node = GetPointer(id);
                        assert (node);
                        NodeId right_id = node->r;
                        RBNode * right = GetPointer(right_id);
                        assert (right);
                        assert (right->c == kRed);
                        node->r = right->l;
                        RBNode * lor = GetPointer(right->l);
                        if (lor) lor->p = id;
                        right->l = id;
                        right->p = node->p;
                        node->p = right_id;

                        right->c = node->c;
                        node->c = kRed;

                        return right_id;
                    }

                    NodeId RotateRight(NodeId id)
                    {
                        RBNode * node = GetPointer(id);
                        assert (node);

                        NodeId left_id = node->l;
                        RBNode * left = GetPointer(left_id);
                        assert (left);
                        assert (left->c == kRed);

                        node->l = left->r;
                        RBNode * rol = GetPointer(left->r);
                        if (rol) rol->p = id;

                        left->r = id;
                        left->p = node->p;
                        node->p = left_id;

                        left->c = node->c;
                        node->c = kRed;

                        return left_id;
                    }

                    void FlipColors(NodeId id)
                    {
                        RBNode * node = GetPointer(id);
                        assert (node);
                        RBNode * left = GetPointer(node->l);
                        assert (left);
                        RBNode * right = GetPointer(node->r);
                        assert (right);

                        node->FlipColor();
                        left->FlipColor();
                        right->FlipColor();
                    }

                    NodeId Put(NodeId node_id, key_type key, mapped_type val)
                    {
                        RBNode * node = GetPointer(node_id);
                        if (node == NULL)
                        {
                            NodeId new_id = ml_->GetSlice();
                            RBNode * new_node = GetPointer(new_id);
                            new_node->Init(key, val);
                            ++md_->size;
                            return new_id;
                        }
                        else
                        {
                            if (comp_(key, node->v.first))
                            {
                                node->l = Put(node->l, key, val);
                                GetPointer(node->l)->p = node_id;
                                node_id = KeepBalance(node_id);
                            }
                            else if(comp_(node->v.first, key))
                            {
                                node->r = Put(node->r, key, val);
                                GetPointer(node->r)->p = node_id;
                                node_id = KeepBalance(node_id);
                            }
                            else
                            {
                                node->v.second = val;
                            }
                            return node_id;
                        }
                    }

                    NodeId Get(NodeId id, key_type key) const
                    {
                        const RBNode * node = GetPointer(id);
                        if (not node) return kInvalidNodeId;
                        else if (comp_(node->v.first, key))
                        {
                            return Get(node->r, key);
                        }
                        else if (comp_(key, node->v.first))
                        {
                            return Get(node->l, key);
                        }
                        else
                        {
                            return id;
                        }
                    }

                    NodeId GetMin(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        assert (node);
                        while (node->l != kInvalidNodeId)
                        {
                            node_id = node->l;
                            node = GetPointer(node_id);
                        }
                        return node_id;
                    }

                    NodeId GetMax(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        assert (node);
                        while (node->r != kInvalidNodeId)
                        {
                            node_id = node->r;
                            node = GetPointer(node_id);
                        }
                        return node_id;
                    }

                    NodeId GetPrev(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        if (node == NULL) return kInvalidNodeId;
                        else if (node->l != kInvalidNodeId)
                        {
                            return GetMax(node->l);
                        }
                        else
                        {
                            NodeId cur = node_id;
                            NodeId parent = node->p;
                            const RBNode * p = GetPointer(parent);
                            while (p and p->l == cur)
                            {
                                cur = parent;
                                parent = p->p;
                                p = GetPointer(parent);
                            }
                            if (p) return parent;
                            else return kInvalidNodeId;
                        }
                    }
                    
                    NodeId GetNext(NodeId node_id) const
                    {
                        const RBNode * node = GetPointer(node_id);
                        if (node == NULL) return kInvalidNodeId;
                        else if (node->r != kInvalidNodeId)
                        {
                            return GetMin(node->r);
                        }
                        else
                        {
                            NodeId cur = node_id;
                            NodeId parent = node->p;
                            const RBNode * p = GetPointer(parent);
                            while (p and p->r == cur)
                            {
                                cur = parent;
                                parent = p->p;
                                p = GetPointer(parent);
                            }
                            if (p) return parent;
                            else return kInvalidNodeId;
                        }
                    }

                    NodeId DeleteMin(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        if (node->l == kInvalidNodeId) /* 3-Node at buttom, ie. node->c == kRed */
                        {
                            assert (node->c == kRed);
#ifndef NDEBUG
                            ::memset(node, 0x0, sizeof(RBNode));
#endif
                            ml_->FreeSlice(node_id);
                            --md_->size;
                            return kInvalidNodeId;
                        }
                        RBNode * left = GetPointer(node->l);
                        assert (left);

                        if (left->c == kBlack)
                        {
                            RBNode * lol = GetPointer(left->l);
                            if (lol == NULL or lol->c == kBlack) /* encountering a 2-Node */
                            {
                                node_id = MoveRedLeft(node_id);
                                node = GetPointer(node_id);
                            }
                        }

                        node->l = DeleteMin(node->l); /* process recursively */
                        return KeepBalance(node_id);
                    }

                    NodeId MoveRedLeft(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);
                        assert (node->c == kRed);

                        RBNode * left = GetPointer(node->l);
                        assert (left);

                        assert (left->c == kBlack);
                        RBNode * lol = GetPointer(left->l);
                        assert (lol == NULL or lol->c == kBlack);

                        FlipColors(node_id);

                        RBNode * right = GetPointer(node->r);
                        assert (right);
                        RBNode * lor = GetPointer(right->l);
                        if (lor and lor->c == kRed)
                        {
                            node->r = RotateRight(node->r);
                            node_id = RotateLeft(node_id);
                            FlipColors(node_id);
                        }

                        return node_id;
                    }

                    NodeId MoveRedRight(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * left = GetPointer(node->l);
                        assert (left);

                        RBNode * right = GetPointer(node->r);
                        assert (right);

                        assert (right->c == kBlack);
                        RBNode * ror = GetPointer(right->r);
                        assert (ror == NULL or ror->c == kBlack);

                        FlipColors(node_id);
                        RBNode * lol = GetPointer(left->l);
                        if (lol and lol->c == kRed)
                        {
                            node_id = RotateRight(node_id);
                            FlipColors(node_id);
                        }
                        return node_id;
                    }

                    NodeId Delete(NodeId node_id, key_type key)
                    {
                        /* pre-condition: current node is not a 2-Node */
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * left = GetPointer(node->l);
                        if (comp_(key, node->v.first))
                        {
                            if (left == NULL) throw 0; /* no such key */
                            if (left->c == kBlack)
                            {
                                RBNode * lol = GetPointer(left->l);
                                if (lol == NULL or lol->c == kBlack) /* encountering 2-Node */
                                {
                                    node_id = MoveRedLeft(node_id);
                                    node = GetPointer(node_id);
                                }
                            }
                            node->l = Delete(node->l, key);
                        }
                        else
                        {
                            if (left != NULL and left->c == kRed)
                            {
                                //borrow from sibling
                                node_id = RotateRight(node_id);
                                node = GetPointer(node_id);
                            }

                            if (not comp_(key, node->v.first) and not comp_(node->v.first, key) and node->r == kInvalidNodeId)
                            {
#ifndef NDEBUG
                                ::memset(node, 0x0, sizeof(RBNode));
#endif
                                ml_->FreeSlice(node_id);
                                --md_->size;
                                return kInvalidNodeId;
                            }

                            RBNode * right = GetPointer(node->r);
                            if (right and right->c == kBlack)
                            {
                                RBNode * lor = GetPointer(right->l);
                                if (lor == NULL or lor->c == kBlack)
                                {
                                    node_id = MoveRedRight(node_id);
                                    node = GetPointer(node_id);
                                }
                            }

                            if (not comp_(key, node->v.first) and not comp_(node->v.first, key))
                            {
                                NodeId successor_id = GetMin(node->r);
                                RBNode * successor = GetPointer(successor_id);
                                assert (successor);
                                assert (successor->l == kInvalidNodeId);
                                /* DON'T assign sucessor->v to node->v and Do DeleteMin, cause this will invalidate iter of successor */
                                NodeId successor_p = successor->p;
                                NodeId successor_r = successor->r;
                                if (successor_id != node->r)
                                {
                                    RBNode * p = GetPointer(node->p);
                                    if (p)
                                    {
                                        if (p->l == node_id) p->l = successor_id;
                                        else p->r = successor_id;
                                    }
                                    RBNode * l = GetPointer(node->l);
                                    if (l) l->p = successor_id;
                                    RBNode * r = GetPointer(node->r);
                                    if (r) r->p = successor_id;

                                    p = GetPointer(successor_p);
                                    assert (p);
                                    assert (p->l == successor_id);
                                    p->l = node_id;
                                    r = GetPointer(successor_r);
                                    if (r) r->p = node_id;
                                    std::swap(successor->p, node->p);
                                    std::swap(successor->l, node->l);
                                    std::swap(successor->r, node->r);
                                    std::swap(successor->c, node->c);
                                }
                                else
                                {
                                    assert (node_id == successor_p);
                                    RBNode * p = GetPointer(node->p);
                                    if (p)
                                    {
                                        if (p->l == node_id) p->l = successor_id;
                                        else p->r = successor_id;
                                    }
                                    RBNode * l = GetPointer(node->l);
                                    if (l) l->p = successor_id;
                                    RBNode * r = GetPointer(successor_r);
                                    if (r) r->p = node_id;

                                    successor->p = node->p;
                                    node->p = successor_id;
                                    successor->l = node->l;
                                    node->l = kInvalidNodeId;
                                    successor->r = node_id;
                                    node->r = successor->r;
                                    std::swap(successor->c, node->c);
                                }

                                node = successor;
                                node_id = successor_id;
                                node->r = DeleteMin(node->r);
                            }
                            else
                            {
                                if (node->r == kInvalidNodeId) throw 0; /* no such key */
                                node->r = Delete(node->r, key);
                            }
                        }
                        return KeepBalance(node_id);
                    }

                public:
                    static std::unique_ptr<RBTree> CreateFrom(void * buf, size_t len)
                    {
                        if (len < sizeof(MetaData)) return NULL;
                        std::unique_ptr<RBTree> ptr(new RBTree);
                        ptr->md_ = reinterpret_cast<MetaData*>(buf);
                        ptr->md_->magic_number = kMagicNumber;
                        ptr->md_->root = kInvalidNodeId;
                        ptr->md_->size = 0;
                        ptr->md_->depth = 0;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + sizeof(MetaData);
                        ptr->ml_.reset(fx::base::MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(RBNode)));
                        if (ptr->ml_ == NULL) return NULL;
                        return std::move(ptr);
                    }

                    static std::unique_ptr<RBTree> RestoreFrom(void * buf, size_t len)
                    {
                        if (len < sizeof(MetaData)) return NULL;
                        MetaData * md = reinterpret_cast<MetaData*>(buf);
                        if (md->magic_number != kMagicNumber) return NULL;

                        std::unique_ptr<RBTree> ptr(new RBTree);
                        ptr->md_ = md;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + sizeof(MetaData);
                        ptr->ml_.reset(fx::base::MemoryList::RestoreFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(RBNode)));
                        if (ptr->ml_ == NULL) return NULL;
                        return std::move(ptr);
                    }

                    iterator begin() 
                    { 
                        if (md_->root == kInvalidNodeId) return iterator(this, kInvalidNodeId);
                        else return iterator(this, GetMin(md_->root));
                    }
                    iterator end() { return iterator(this, kInvalidNodeId); }

                    const_iterator begin() const
                    {
                        if (md_->root == kInvalidNodeId) return const_iterator(this, kInvalidNodeId);
                        else return const_iterator(this, GetMin(md_->root));
                    }

                    const_iterator end() const { return const_iterator(this, kInvalidNodeId); }

                    size_t size() const 
                    {
                        return md_->size;
                    }

                    size_t max_size() const
                    {
                        return ml_->capacity();
                    }

                    std::pair<iterator, bool> insert(const std::pair<key_type, mapped_type> val)
                    {
                        auto iter = find(val.first);
                        if (iter == end())
                        {
                            if (size() == max_size()) return std::make_pair(end(), false);
                            md_->root = Put(md_->root, val.first, val.second);
                            RBNode * node = GetPointer(md_->root);
                            assert (node);
                            if (node->c == kRed)
                            {
                                node->c = kBlack;
                                ++md_->depth;
                            }
                            return std::make_pair(find(val.first), true);
                        }
                        return std::make_pair(iter, false);
                    }

                    mapped_type & operator[](key_type key)
                    {
                        auto iter = find(key);
                        if (iter == end())
                        {
                            mapped_type m;
                            auto pair = insert(std::make_pair(key, m));
                            if (pair.first == end()) throw std::bad_alloc();
                            else return pair.first->second;
                        }
                        else
                        {
                            return iter->second;
                        }
                    }

                    size_t erase(key_type key)
                    {
                        if (md_->root == kInvalidNodeId) return 0;
                        RBNode * root = GetPointer(md_->root);
                        root->c = kRed;
                        size_t count = 0;
                        try
                        {
                            md_->root = Delete(md_->root, key);
                            count = 1;
                        }
                        catch (int)
                        {
                            count = 0;
                        }
                        if (md_->root != kInvalidNodeId) GetPointer(md_->root)->c = kBlack;
                        return count;
                        //auto iter = find(key);
                        //return erase(iter);
                    }

                    size_t erase(const const_iterator& iter)
                    {
                        assert (iter.tree_ == this);
                        if (iter == end()) return 0;
                        return erase(iter->first);
                    }
                    //    if (md_->root == kInvalidNodeId) return 0;
                    //    else if (iter == end()) return 0;
                    //    else
                    //    {
                    //        NodeId node_id = iter.id_;
                    //        RBNode * node = GetPointer(node_id);
                    //        RBNode * p = GetPointer(node->p);
                    //        if (p)
                    //        {
                    //            if (p->l == node_id)
                    //            {
                    //                p->l = Delete(node_id, iter->first);
                    //                if (p->l != kInvalidNodeId) p->l = KeepBalance(p->l);
                    //            }
                    //            else
                    //            {
                    //                p->r = Delete(node_id, iter->first);
                    //                if (p->r != kInvalidNodeId) p->r = KeepBalance(p->r);
                    //            }
                    //        }
                    //        else
                    //        {
                    //            md_->root = Delete(node_id, iter->first);
                    //        }
                    //        return 1;
                    //    }
                    //}

                    void clear()
                    {
                        ml_->clear();
                        md_->size = 0;
                        md_->depth = 0;
                        md_->root = kInvalidNodeId;
                    }

                    size_t count(key_type key)
                    {
                        return find(key) == end() ? 0 : 1;
                    }

                    iterator find(key_type key)
                    {
                        return iterator(this, Get(md_->root, key));
                    }

                    const_iterator find(key_type key) const
                    {
                        return const_iterator(this, Get(md_->root, key));
                    }

                    void LevelOrderTraversal()
                    {
                        if (md_->root == kInvalidNodeId) return;
                        std::vector<NodeId> nodes;
                        nodes.push_back(md_->root);

                        size_t cur = 0, last = 1;

                        while (cur < nodes.size())
                        {
                            last = nodes.size();

                            while (cur < last)
                            {
                                NodeId now = nodes[cur];
                                RBNode * node = GetPointer(now);
                                assert (node);

                                std::cout << node->v.first << "(" << (node->c == kRed ? "red" : "black") << " " 
                                    << now << " " << node->p << " " << node->l << " " << node->r << ")\t";
                                if (node->l != kInvalidNodeId) nodes.push_back(node->l);
                                if (node->r != kInvalidNodeId) nodes.push_back(node->r);

                                ++cur;
                            }
                            std::cout << "\n********************************************************************************\n";
                        }
                        std::cout << "********************************************************************************\n";
                    }
                private:
                    MetaData * md_;
                    std::unique_ptr<fx::base::MemoryList> ml_;
                    key_compare comp_;
            };
        }
    }
}

#endif   /* ----- #ifndef __RBTREE_H__  ----- */
