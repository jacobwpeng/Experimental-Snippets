/*
 * =====================================================================================
 *
 *       Filename:  processor.h
 *        Created:  11/24/2013 08:16:18 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  base class for all mathematical calculations
 *
 * =====================================================================================
 */


#ifndef  processor_INC
#define  processor_INC

class IProcessor
{
    public:
        virtual ~IProcessor() {}
        virtual int DoCalculate(int a, int b) = 0;
};

#endif   /* ----- #ifndef processor_INC  ----- */


