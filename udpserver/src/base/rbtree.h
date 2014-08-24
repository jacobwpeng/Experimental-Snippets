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
#include <memory>
#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pod.hpp>
#include "memory_list.h"

namespace fx
{
    namespace base
    {
        namespace container
        {
            template <typename _Key, typename _Value, typename KeyEnable = void, typename ValueEnable = void>
            class RBTree;

            template <typename _Key, typename _Value>
            class RBTree<_Key, _Value, typename boost::enable_if <boost::is_pod<_Key> >::type , typename boost::enable_if< boost::is_pod<_Value> >::type>
            {
                public:
                    typedef _Key KeyType;
                    typedef _Value ValueType;
                private:
                    typedef fx::base::MemoryList::SliceId NodeId;
                    static const NodeId kInvalidNodeId = fx::base::MemoryList::kInvalidSliceId;
                    static const int64_t kMagicNumber = 1050165255719967046;
                    struct MetaData
                    {
                        int64_t magic_number;
                        NodeId root;
                        size_t size;
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
                        KeyType k;
                        ValueType v;

                        void Init(KeyType key, ValueType val)
                        {
                            c = kRed;
                            p = l = r = kInvalidNodeId;
                            k = key;
                            v = val;
                        }

                        void FlipColor()
                        {
                            c = (c == kRed ? kBlack : kRed);
                        }
                    };

                    RBTree()
                        :depth_(0)
                    {
                    }

                private:
                    RBNode * GetPointer(NodeId id)
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<RBNode*>( ml_->GetBuffer(id) );
                    }

                    NodeId KeepBalance(NodeId node_id)
                    {
                        RBNode * node = GetPointer(node_id);
                        assert (node);

                        RBNode * right = GetPointer(node->r);
                        if (right and right->c == kRed)
                        {
                            node_id = RotateLeft(node_id);
                        }

                        node = GetPointer(node_id);
                        assert (node);

                        RBNode * left = GetPointer(node->l);
                        if (left and left->c == kRed)
                        {
                            RBNode * lol = GetPointer(left->l);
                            if (lol and lol->c == kRed)
                            {
                                node_id = RotateRight(node_id);
                            }
                        }

                        node = GetPointer(node_id);
                        assert (node);
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

                    NodeId Put(NodeId node_id, KeyType key, ValueType val)
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
                            if (key > node->k)
                            {
                                node->r = Put(node->r, key, val);
                                node_id = KeepBalance(node_id);
                            }
                            else if (key < node->k)
                            {
                                node->l = Put(node->l, key, val);
                                node_id = KeepBalance(node_id);
                            }
                            else
                            {
                                node->v = val;
                            }
                            return node_id;
                        }
                    }

                public:
                    static RBTree * CreateFrom(void * buf, size_t len)
                    {
                        if (len < sizeof(MetaData)) return NULL;
                        std::auto_ptr<RBTree> ptr(new RBTree);
                        ptr->md_ = reinterpret_cast<MetaData*>(buf);
                        ptr->md_->magic_number = kMagicNumber;
                        ptr->md_->root = kInvalidNodeId;
                        ptr->md_->size = 0;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + sizeof(MetaData);
                        ptr->ml_.reset(fx::base::MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(RBNode)));
                        if (ptr->ml_ == NULL) return NULL;
                        return ptr.release();
                    }

                    static RBTree * RestoreFrom(void * buf, size_t len)
                    {
                        if (len < sizeof(MetaData)) return NULL;
                        MetaData * md = reinterpret_cast<MetaData*>(buf);
                        if (md->magic_number != kMagicNumber) return NULL;

                        std::auto_ptr<RBTree> ptr(new RBTree);
                        ptr->md_ = md;
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + sizeof(MetaData);
                        ptr->ml_.reset(fx::base::MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(MetaData), sizeof(RBNode)));
                        if (ptr->ml_ == NULL) return NULL;
                        return ptr.release();
                    }

                    size_t size() const 
                    {
                        return md_->size;
                    }

                    size_t depth() const
                    {
                        return depth_;
                    }

                    size_t capacity() const
                    {
                        return ml_->capacity();
                    }

                    bool Put(KeyType key, ValueType val)
                    {
                        if (ml_->size() == ml_->capacity()) return false;

                        md_->root = Put(md_->root, key, val);
                        RBNode * node = GetPointer(md_->root);
                        assert (node);
                        if (node->c == kRed)
                        {
                            node->c = kBlack;
                            ++depth_;
                        }
                        return true;
                    }

                private:
                    MetaData * md_;
                    boost::scoped_ptr<fx::base::MemoryList> ml_;
                    size_t depth_;
            };
        }
    }
}

#endif   /* ----- #ifndef __RBTREE_H__  ----- */
