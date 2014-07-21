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

D::E::C & D::E::SayHello()
{
    static D::E::C c;
    std::cout << "Hello" << '\n';
    return c;
}

namespace fx
{
    void DynamicArray::Append(int element)
    {
        return;
    }

    int DynamicArray::Size() const
    {
        return 0;
    }

    bool DynamicArray::Empty() const
    {
        return true;
    }

    LUA_EXPORT int DynamicArray::Get(const std::vector<int> * ptr, 
            std::vector< pair<int, std::string> > & vecs) const
    {
        return 0;
    }

    std::map< std::vector< pair<int, std::string> >, std::string> & DynamicArray::This(
            const std::string ** ppstr,
            std::string * pstr,
            const std::string& str,
            std::string s)
    {
        static std::map< std::vector< pair<int, std::string> >, std::string> m;
        return m;
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
}
