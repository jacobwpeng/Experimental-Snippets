/*
 * =====================================================================================
 *
 *       Filename:  l_aio.h
 *        Created:  12/10/2013 10:27:38 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  async IO for lua
 *
 * =====================================================================================
 */

#ifndef  __L_AIO__
#define  __L_AIO__

#include <aio.h>
#include <lua.h>

static const size_t max_info_size = 128;
typedef struct { struct aiocb m_aiocb; char* buf; int buf_len; int idx; void* handle;} info_t;
typedef struct { lua_State* L; int fd; int mode; info_t* infos[max_info_size]; } handle_t;

#endif   /* ----- #ifndef __L_AIO__  ----- */

