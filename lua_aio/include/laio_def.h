/*
 * =====================================================================================
 *
 *       Filename:  laio_def.h
 *        Created:  05/20/14 16:48:26
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#ifndef  __LAIO_DEF__
#define  __LAIO_DEF__

static const size_t max_info_size = 16;

typedef struct info_s info_t;
typedef struct handle_s handle_t;
struct handle_s
{
    lua_State* L;
    int fd;
    int mode;
    info_t* infos[max_info_size];
    unsigned gc;
};

struct info_s 
{ 
    lua_State* L; 
    struct aiocb m_aiocb; 
    char* buf; 
    int buf_len; 
    int idx; 
    handle_t* handle; 
    int aio_ret;
    int ret; 
    unsigned done : 1;
    unsigned gc;
};

int first_slot(info_t** infos, int size);
void cleanup_info(info_t* info);
void cleanup_handle(handle_t* handle);

#endif   /* ----- #ifndef __LAIO_DEF__  ----- */
