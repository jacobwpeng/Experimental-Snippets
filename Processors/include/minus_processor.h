/*
 * =====================================================================================
 *
 *       Filename:  minus_processor.h
 *        Created:  11/24/2013 02:20:47 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  minus_processor_INC
#define  minus_processor_INC

#include "processor.h"

class MinusProcessor : public IProcessor
{
    public:
        virtual ~MinusProcessor() {}
        virtual int DoCalculate(int a, int b);
};
#endif   /* ----- #ifndef minus_processor_INC  ----- */

