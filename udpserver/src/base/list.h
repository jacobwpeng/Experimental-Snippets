/*
 * =====================================================================================
 *
 *       Filename:  list.h
 *        Created:  08/25/14 20:37:13
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __LIST_H__
#define  __LIST_H__

#include <cassert>
#include <memory>
#include <type_traits>
#include "memory_list.h"

namespace fx
{
    namespace base
    {
        namespace container
        {
            template <typename T, typename Enable = void>
            class List;

            template <typename T>
            class List<T, typename std::enable_if< std::is_pod<T>::value >::type >
            {
                public:
                    typedef fx::base::MemoryList::SliceId NodeId;
                    typedef T value_type;
                    class iterator;
                    class const_iterator;

                private:
                    typedef List<T, typename std::enable_if< std::is_pod<T>::value >::type > Self;
                    static const int64_t kMagicNumber = 8522338164747840960;

                private:
                    struct Header
                    {
                        int64_t magic_number;
                        size_t size;
                        NodeId head;
                        NodeId tail;

                        void Init()
                        {
                            size = 0;
                            magic_number = kMagicNumber;
                            head = tail = kInvalidNodeId;
                        }
                    };

                    struct Node
                    {
                        NodeId prev;
                        NodeId next;
                        T val;
                    };

                    List()
                    {
                    }

                    Node * GetNode(NodeId id)
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<Node*>(ml_->GetBuffer(id));
                    }

                    const Node * GetNode(NodeId id) const
                    {
                        if (id == kInvalidNodeId) return NULL;
                        return reinterpret_cast<Node*>(ml_->GetBuffer(id));
                    }

                    template<typename DerivedType, typename ContainerType, typename Pointer, typename Reference>
                    class IteratorBase
                    {
                        public:
                            DerivedType& operator++()
                            {
                                assert (c_);
                                assert (id_ != kInvalidNodeId);
                                id_ = c_->GetNode(id_)->next;
                                return static_cast<DerivedType&>(*this);
                            }

                            const DerivedType operator++(int)
                            {
                                assert (c_);
                                assert (id_ != kInvalidNodeId);
                                auto result = static_cast<DerivedType&>(*this);
                                ++*this;
                                return result;
                            }

                            template<typename DerivedType2, typename ContainerType2, typename Pointer2, typename Reference2>
                            bool operator == (const IteratorBase<DerivedType2, ContainerType2, Pointer2, Reference2> & rhs) const
                            {
                                return c_ == rhs.c_ and id_ == rhs.id_;
                            }

                            template<typename DerivedType2, typename ContainerType2, typename Pointer2, typename Reference2>
                            bool operator != (const IteratorBase<DerivedType2, ContainerType2, Pointer2, Reference2> & rhs) const
                            {
                                return !(*this == rhs);
                            }

                            Reference operator* () const { return c_->GetNode(id_)->val; }
                            Pointer operator-> () const { return &**this; }

                        protected:
                            template<typename DerivedType2, typename ContainerType2, typename Pointer2, typename Reference2>
                            friend class IteratorBase;

                            IteratorBase(ContainerType * container = NULL, NodeId id = kInvalidNodeId)
                                :c_(container), id_(id)
                            {
                            }

                        protected:
                            ContainerType * c_;
                            NodeId id_;
                    };
                    template<typename DerivedType, typename ContainerType, typename Pointer, typename Reference>
                    friend class IteratorBase;

                public:
                    static const NodeId kInvalidNodeId = fx::base::MemoryList::kInvalidSliceId;
                    static const size_t kHeaderSize = sizeof(Header);
                    static std::unique_ptr<Self> CreateFrom(void * buf, size_t len)
                    {
                        if (len < kHeaderSize) return NULL;

                        std::unique_ptr<List> ptr(new List);
                        ptr->header_ = reinterpret_cast<Header*>(buf);
                        ptr->header_->Init();
                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + kHeaderSize;
                        ptr->ml_.reset (MemoryList::CreateFrom(reinterpret_cast<void*>(start), len - sizeof(Header), sizeof(Node)));
                        if (ptr->ml_ == NULL) return NULL;
                        return std::move(ptr);
                    }

                    static std::unique_ptr<Self> RestoreFrom(void * buf, size_t len)
                    {
                        if (len < kHeaderSize) return NULL;

                        std::unique_ptr<List> ptr(new List);
                        Header * md = reinterpret_cast<Header*>(buf);
                        if (md->magic_number != kMagicNumber) return NULL;
                        ptr->header_ = md;

                        uintptr_t start = reinterpret_cast<uintptr_t>(buf) + kHeaderSize;
                        ptr->ml_.reset (MemoryList::RestoreFrom(reinterpret_cast<void*>(start), len - sizeof(Header), sizeof(Node)));
                        if (ptr->ml_ == NULL) return NULL;
                        return std::move(ptr);
                    }

                    class const_iterator : public std::iterator<std::bidirectional_iterator_tag, value_type>, 
                                           public IteratorBase<const_iterator, const Self, const value_type*, const value_type&>
                    {
                        public:
                            const_iterator() {}

                            const_iterator(iterator iter)
                                :IteratorBase<const_iterator, const Self, const value_type*, const value_type&>(iter.container_, iter.id_){}
                            const_iterator(const Self * container, NodeId id)
                                :IteratorBase<const_iterator, const Self, const value_type*, const value_type&>(container, id) { } 
                            friend class List;
                    };
                    friend class const_iterator;

                    class iterator : public std::iterator<std::bidirectional_iterator_tag, value_type>, 
                                     public IteratorBase<iterator, Self, value_type*, value_type&>
                    {
                        public:
                            iterator() {}

                        private:
                            iterator(Self * container, NodeId id) :IteratorBase<iterator, Self, value_type*, value_type&>(container, id) { } 
                            friend class List;
                            friend class const_iterator;
                    };
                    friend class iterator;

                    iterator begin() { return iterator(this, header_->head); }
                    const_iterator begin() const { return const_iterator(this, header_->head); }

                    iterator end() { return iterator(this, kInvalidNodeId); }
                    const_iterator end() const { return const_iterator(this, kInvalidNodeId); }

                    T Unlink(NodeId id)
                    {
                        assert (id != kInvalidNodeId);
                        Node * node = GetNode(id);
                        Node * prev = GetNode(node->prev);
                        Node * next = GetNode(node->next);
                        if (prev)
                        {
                            prev->next = node->next;
                        }
                        else
                        {
                            header_->head = node->next;
                        }
                        if (next)
                        {
                            next->prev = node->prev;
                        }
                        else
                        {
                            header_->tail = node->prev;
                        }
                        --header_->size;
                        T val = node->val;
                        ml_->FreeSlice(id);
                        return val;
                    }

                    iterator erase(iterator pos)
                    {
                        assert (pos != end());
                        auto next = pos;
                        ++next;
                        Unlink(pos.id_);
                        return next;
                    }

                    iterator erase(iterator first, iterator last)
                    {
                        auto res = last;
                        ++res;
                        auto pos = first;
                        while (pos != last)
                        {
                            erase(pos);
                            ++pos;
                        }
                        return res;
                    }

                    NodeId push_front(T val)
                    {
                        if (ml_->full()) return kInvalidNodeId;

                        NodeId id = ml_->GetSlice();
                        Node * node = GetNode(id);
                        node->val = val;

                        if (header_->head == kInvalidNodeId)
                        {
                            assert (header_->tail == kInvalidNodeId);
                            header_->head = id;
                            header_->tail = id;
                            node->prev = kInvalidNodeId;
                            node->next = kInvalidNodeId;
                        }
                        else
                        {
                            Node * front = GetNode(header_->head);
                            assert (front);
                            assert (front->prev = kInvalidNodeId);
                            front->prev = id;
                            node->prev = kInvalidNodeId;
                            node->next = header_->head;
                            header_->head = id;
                        }
                        ++header_->size;
                        return id;
                    }

                    NodeId push_back(T val)
                    {
                        if (ml_->full()) return kInvalidNodeId;

                        NodeId id = ml_->GetSlice();
                        Node * node = GetNode(id);
                        node->val = val;

                        if (header_->tail == kInvalidNodeId)
                        {
                            assert (header_->head == kInvalidNodeId);
                            header_->tail = id;
                            header_->head = id;
                            node->prev = kInvalidNodeId;
                            node->next = kInvalidNodeId;
                        }
                        else
                        {
                            Node * last = GetNode(header_->tail);
                            assert (last);
                            assert (last->next = kInvalidNodeId);
                            last->next = id;
                            node->prev = header_->tail;
                            node->next = kInvalidNodeId;
                            header_->tail = id;
                        }
                        ++header_->size;
                        return id;
                    }

                    T pop_front()
                    {
                        assert (header_->head != kInvalidNodeId);
                        assert (header_->tail != kInvalidNodeId);

                        return Unlink(header_->head);
                    }

                    T pop_back()
                    {
                        assert (header_->head != kInvalidNodeId);
                        assert (header_->tail != kInvalidNodeId);

                        return Unlink(header_->tail);
                    }

                    T back()
                    {
                        assert (header_->tail != kInvalidNodeId);
                        return GetNode(header_->tail)->val;
                    }

                    T front()
                    {
                        assert (header_->head != kInvalidNodeId);
                        return GetNode(header_->head)->val;
                    }

                    size_t size() const 
                    { 
                        assert (ml_->size() == header_->size);
                        return header_->size; 
                    }
                    size_t capacity() const { return ml_->capacity(); }
                    bool full() const { return size() == capacity(); }

                    void clear()
                    {
                        ml_->clear();
                        header_->size = 0;
                        header_->head = kInvalidNodeId;
                        header_->tail = kInvalidNodeId;
                    }

                private:
                    Header * header_;
                    std::unique_ptr<fx::base::MemoryList> ml_;
            };
        }
    }
}

#endif   /* ----- #ifndef __LIST_H__  ----- */
