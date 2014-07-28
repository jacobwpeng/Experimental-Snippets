/*
 * =====================================================================================
 *
 *       Filename:  dynamic_array.cc
 *        Created:  07/17/14 15:27:40
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "dynamic_array.h"
#include <iostream>
#include <cassert>

namespace fx
{
    DynamicArray::DynamicArray()
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    DynamicArray::DynamicArray(int i)
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    void DynamicArray::Append(int element)
    {
        v_.push_back(element);
        return;
    }

    int DynamicArray::Size() const
    {
        return v_.size();
    }

    bool DynamicArray::Empty() const
    {
        return true;
    }

    int DynamicArray::Get(int index) const
    {
        assert (index >= 1);
        assert (index - 1< v_.size());
        return v_[index - 1];
    }

    std::string DynamicArray::GetMessage()
    {
        return "Hello, World!";
    }

    void DynamicArray::PrintMessage(const char* str)
    {
        std::cout << str << '\n';
    }

    //DynamicArray * DynamicArray::New()
    //{
    //    return new DynamicArray;
    //}

    //void DynamicArray::Delete(DynamicArray * ptr)
    //{
    //    delete ptr;
    //}

    void DynamicArray::TestOverload(int i)
    {
        std::cout << __PRETTY_FUNCTION__ << '\t' << i << '\n';
    }

    void DynamicArray::TestOverload(std::string)
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }

    void DynamicArray::TestOverload(unsigned)
    {
        std::cout << __PRETTY_FUNCTION__ << '\n';
    }
}
