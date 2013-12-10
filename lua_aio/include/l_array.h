/*
 * =====================================================================================
 *
 *       Filename:  l_array.h
 *        Created:  12/10/2013 09:22:13 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  export array for lua
 *
 * =====================================================================================
 */


#ifndef  __L_ARRAY__
#define  __L_ARRAY__

#include <lua.h>

typedef struct { size_t size; int* arr; } array;

#endif   /* ----- #ifndef __L_ARRAY__  ----- */

