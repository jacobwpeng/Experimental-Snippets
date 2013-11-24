/*
 * =====================================================================================
 *
 *       Filename:  add_processor.h
 *        Created:  11/24/2013 09:00:38 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  do add
 *
 * =====================================================================================
 */

#ifndef  add_processor_INC
#define  add_processor_INC

#include "processor.h"

class AddProcessor : public IProcessor
{
    public:
        virtual ~AddProcessor() {}
        virtual int DoCalculate(int a, int b);
}

#endif   /* ----- #ifndef add_processor_INC  ----- */
;


