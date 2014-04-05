/*
 * =====================================================================================
 *       Filename:  map.hpp
 *        Created:  15:24:10 Apr 02, 2014
 *         Author:  jacobwpeng
 *          Email:  jacobwpeng@tencent.com
 *    Description:  STL-Like HashMap implemented using Judy Array
 *
 * =====================================================================================
 */

#ifndef  __MAP_H__
#define  __MAP_H__

#include <cassert>
#include <memory>

#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>

#include <Judy.h>

namespace FX
{
    template<typename T>
    struct Converter
    {
        Word_t operator()(typename boost::call_traits<T>::param_type val) { return val; }
    };

    template<typename k, typename v, 
        typename key_converter = Converter<k>, typename alloc = std::allocator<v> >
    class Map
    {
        public:
            typedef k key_type;
            typedef v mapped_type;
            typedef std::pair<k, v> value_type;
            typedef key_converter key_converter_type;
            typedef alloc allocator_type;
            typedef size_t size_type;

            typedef typename boost::call_traits<key_type>::param_type key_param_type;

            struct Iterator
            {
                typedef std::input_iterator_tag iterator_category;
                typedef mapped_type value_type;
                typedef void difference_type;
                typedef typename boost::add_reference<mapped_type>::type reference;
                typedef typename boost::add_pointer<mapped_type>::type pointer;
                Iterator(Pvoid_t arr, Word_t raw_key, mapped_type val)
                    :arr_(arr), raw_key_(raw_key), val_(val)
                {
                }

                Iterator()
                {
                    Reset();
                }

                void operator++()
                {
                    if( Valid() )
                    {
                        Pvoid_t ptr_val;
                        JLN(ptr_val, arr_, raw_key_);
                        if( ptr_val == NULL ) Reset();
                        else
                        {
                            Word_t ptr = *static_cast<PWord_t>(ptr_val);
                            val_ = *(reinterpret_cast<mapped_type*>(ptr));
                        }
                    }
                }

                Iterator operator++(int)
                {
                    Iterator res(*this);
                    ++res;
                    return res;
                }

                bool operator==(const Iterator& rhs) const
                {
                    return arr_ == rhs.arr_ && raw_key_ == rhs.raw_key_;
                }

                bool operator!=(const Iterator& rhs) const
                {
                    return !(this->operator==(rhs));
                }

                value_type operator*() const
                {
                    assert( Valid() );
                    return val_;
                }

                private:
                    bool Valid() const
                    {
                        return arr_ != NULL;
                    }
                    void Reset()
                    {
                        arr_ = NULL;
                        raw_key_ = 0;
                        val_ = mapped_type();
                    }

                    template<typename _k, typename _v, typename _converter, typename _alloc>
                    friend class Map;

                private:
                    Pvoid_t arr_;
                    Word_t raw_key_;
                    mapped_type val_;
            };

            typedef Iterator iterator;

            Map()
            :arr_(NULL), size_(0) {}

            ~Map()
            {
                Pvoid_t raw_ptr = NULL;
                Word_t addr;
                Word_t idx = 0;
                int ret;

                JLF(raw_ptr, arr_, idx); 
                while( raw_ptr != NULL )
                {
                    mapped_type * obj_ptr;
                    addr = *static_cast<PWord_t>(raw_ptr);
                    obj_ptr = reinterpret_cast<mapped_type*>(addr);
                    allocator_.destroy( obj_ptr );
                    allocator_.deallocate( obj_ptr, 1 );

                    JLD( ret, arr_, idx );
                    JLN( raw_ptr, arr_, idx );
                }
                arr_ = NULL;
            }

            iterator find( key_param_type key )
            {
                Pvoid_t ptr_val;
                Word_t idx = key_converter_type()(key);
                JLG(ptr_val, arr_, idx);
                if( ptr_val == NULL ) return iterator();
                else
                {
                    Word_t ptr = *static_cast<PWord_t>(ptr_val);
                    mapped_type * ptr_obj = reinterpret_cast<mapped_type*>(ptr);
                    return iterator( arr_, idx, *ptr_obj );
                }
            }

            std::pair<iterator, bool> insert( const value_type& val )
            {
                iterator iter = this->find(val.first);
                if( iter != this->end() ) return std::make_pair(iter, false);
                else
                {
                    Word_t idx = key_converter_type()(val.first);
                    Pvoid_t raw_ptr;
                    JLI( raw_ptr, arr_, idx );
                    if( raw_ptr == PJERR )
                    {
                        /* TODO : throw an exception */
                        assert( false );
                        return std::make_pair( iterator(), false );
                    }

                    Word_t addr = *static_cast<PWord_t>(raw_ptr);
                    if( addr == 0 )
                    {
                        /* insert succeed */
                        mapped_type * ptr = allocator_.allocate( 1 );
                        allocator_.construct( ptr, val.second );
                        *static_cast<PWord_t>(raw_ptr) = reinterpret_cast<Word_t>(ptr);
                        ++size_;
                        return std::make_pair( iterator(arr_, idx, *ptr), true );
                    }
                    else
                    {
                        /* TODO : throw an exception */
                        assert( false );
                        return std::make_pair( iterator(), false );
                    }
                }
            }

            void erase( const iterator& pos )
            {
                assert( pos.Valid() );
                assert( pos.arr_ == arr_ );
                int ret = 0;
                JLD(ret, arr_, pos.raw_key_ );
                assert( ret == 1 );
                --size_;
            }

            size_type erase( key_param_type key )
            {
                Iterator iter = this->find(key);
                if( iter == this->end() ) return 0;
                else
                {
                    erase(iter);
                    return 1;
                }
            }

            size_type size() const { return this->size_; }

            unsigned long ByteSize() const
            {
                Word_t used;
                JLMU(used, arr_);
                return used;
            }

            iterator begin()
            {
                Pvoid_t ptr_val;
                Word_t idx = 0;
                JLF(ptr_val, arr_, idx);
                if( ptr_val == NULL ) return iterator();
                else
                {
                    Word_t ptr = *static_cast<PWord_t>(ptr_val);
                    mapped_type * ptr_obj = reinterpret_cast<mapped_type*>(ptr);
                    return iterator( arr_, idx, *ptr_obj );
                }
            }

            iterator end()
            {
                return iterator();
            }

        private:
            Pvoid_t arr_;
            allocator_type allocator_;
            size_t size_;
    };
}

#endif   /* ----- #ifndef __MAP_H__----- */
