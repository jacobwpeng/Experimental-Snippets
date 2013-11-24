/*
 * =====================================================================================
 *
 *       Filename:  processor_mgr.cc
 *        Created:  11/24/2013 09:06:35 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "processor_mgr.h"
#include "add_processor.h"
#include "minus_processor.h"
#include <string>

using std::string;

IProcessor* CreateProcessor(const char* name)
{
    string processor_name( name );
    IProcessor* p = NULL;

    if( processor_name == "add" )
        p = new AddProcessor;
#ifdef ENABLE_MINUS
    else if( processor_name == "minus" )
        p = new MinusProcessor;
#endif

    return p;
}
void DestroyProcessor(IProcessor* p)
{
    delete p;
}

