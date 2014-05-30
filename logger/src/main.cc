/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  05/27/14 20:05:43
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "logger.h"
#include <ostream>

struct Base
{
};

std::ostream& operator<<(std::ostream& os, const Base& base)
{
    os << "Base";
    return os;
}

int main()
{
    Logger::Init();
    //LOG(DEBUG) << Base();
    return 0;
}
