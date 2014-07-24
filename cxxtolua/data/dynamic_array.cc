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

D::E::C & D::E::SayHello()
{
    static D::E::C c;
    std::cout << "Hello" << '\n';
    return c;
}

namespace fx
{
    DynamicArray::DynamicArray()
    {

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

    int DynamicArray::Size(int i) const
    {
        return i;
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

    std::string DynamicArray::SayHello() const
    {
        return "Hello, World!";
    }

    DynamicArray * DynamicArray::New()
    {
        return new DynamicArray;
    }

    void DynamicArray::Delete(DynamicArray * ptr)
    {
        delete ptr;
    }
}
