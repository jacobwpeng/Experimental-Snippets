/*
 * =====================================================================================
 *
 *       Filename:  processor_mgr.h
 *        Created:  11/24/2013 09:05:47 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  processor_mgr_INC
#define  processor_mgr_INC

#include "processor.h"

extern "C"
{
    IProcessor* CreateProcessor(const char* name);
    void DestroyProcessor(IProcessor* p);
}

#endif   /* ----- #ifndef processor_mgr_INC  ----- */

