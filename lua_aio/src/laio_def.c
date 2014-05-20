/*
 * =====================================================================================
 *
 *       Filename:  laio_def.c
 *        Created:  05/20/14 16:50:05
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */
#include <assert.h>

int first_slot(info_t** infos, int size)
{
    int i = 0;
    while( i < size && infos[i] != NULL ) ++i;
    return i == size ? -1 : i;
}

void cleanup_info(info_t* info)
{
    assert(info);
    if(info->gc == 1) return;
    free(info->buf);
    info->handle->infos[info->idx] = NULL;
    info->gc = 1;
}

void cleanup_handle(handle_t* handle)
{
    assert(handle);
    int i = 0;

    if( handle->gc == 1 ) return;               /* for destroyed manually */
    for(; i != max_info_size; ++i)
    {
        if( handle->infos[i] != NULL ) cleanup_info(handle->infos[i]);
    }
    close(handle->fd);
    handle->gc = 1;
}
