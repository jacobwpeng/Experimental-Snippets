/*
 * =====================================================================================
 *
 *       Filename:  l_aio.c
 *        Created:  12/10/2013 10:29:45 AM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  async IO for lua
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <aio.h>
#include <errno.h>
#include <signal.h>
#include <lauxlib.h>
#include "l_aio.h"

#define checkhandle(L) \
    (handle_t*) luaL_checkudata(L, 1, metatable_name);

static const char* metatable_name = "l_aio_m";
static int sig_no;
static sigset_t sig_blocked;

#define dump_state(L) \
    printf("func : %s, line : %d\n", __FUNCTION__, __LINE__); dump_state_internal(L);

static int protect_resume(lua_State* L)
{
    int status = lua_status(L);
    if( status == LUA_YIELD )
        status = lua_resume(L, 1);
    return status;
}

static void dump_state_internal(lua_State* L)
{
    int i;
    int top = lua_gettop(L);
    for( i = 1; i != top + 1; ++i)
    {
        printf("index = %d, type : %s\n", i, luaL_typename(L, i));
        if( lua_isstring(L, i) ) printf("value : %s\n", lua_tostring(L, i));
    }
    printf("********************************************************************************\n");
}

static int first_slot(info_t** infos, int size)
{
    int i = 0;
    while( i < size && infos[i] != NULL ) ++i;
    return i == size ? -1 : i;
}

static void cleanup_info(info_t* info)
{
    handle_t* handle;
    assert(info);

    free( info->buf );
    handle = info->handle;
    handle->infos[ info->idx ] = NULL;
}

static void cleanup_handle(handle_t* handle)
{
    assert(handle);
    printf("cleanup_handle\n");
    int i;
    printf("close return : %d\n", close(handle->fd));
    for( i = 0; i != max_info_size; ++i )
    {
        if( handle->infos[i] != NULL )
        {
            cleanup_info( handle->infos[i] );
        }
    }
}

static void aio_completion_handler(int signo, siginfo_t* sig_info, void* ctx)
{
    int ret;
    int error_ret;
    struct aiocb* req;
    handle_t* handle;
    info_t* info;

    sigprocmask( SIG_BLOCK, &sig_blocked, NULL);
    if( sig_info->si_signo != sig_no) return;

    info = (info_t*)sig_info->si_value.sival_ptr;
    handle = (handle_t*)info->handle;
    req = &info->m_aiocb;

    error_ret = aio_error(req);
    ret = aio_return(req);

    if( error_ret != 0 || ret < 0 )
    {
        printf("Error return, error_ret = %d, ret = %d\n", error_ret, ret);
        ret = protect_resume(info->L);
    }
    else
    {
        if( handle->mode == O_RDONLY )
        {
            if( ret == 0 )
            {
                lua_pushnil(info->L);
                cleanup_info(info);
            }
            else
            {
                //lua_pushliteral(info->L, "read done");
                lua_pushlstring(info->L, info->buf, ret);
            }
            ret = protect_resume(info->L);
        }
        else
        {
            lua_pushinteger(info->L, ret);
            ret = protect_resume(info->L);
        }
        if( ret != LUA_YIELD && ret != 0 )
        {
            printf("top = %d, ret = %d\n", lua_gettop(info->L), ret);
            dump_state(info->L);
            abort();
        }
    }
    sigprocmask( SIG_UNBLOCK, &sig_blocked, NULL);
}

static int laio_new_handle(lua_State* L)
{
    int i;
    int mode = -1;
    int fd;
    char m;
    size_t len;
    size_t nBytes;
    handle_t* handle;
    const char* mode_str;
    const char* filename;

    filename = lua_tolstring(L, 1, &len);
    luaL_argcheck(L, len != 0, 1, "invalid name");

    mode_str = lua_tolstring(L, 2, &len);
    luaL_argcheck(L, len == 1, 2, "invalid mode");

    m = mode_str[0];
    switch(m)
    {
        case 'a':
            mode = O_APPEND;
            break;
        case 'r':
            mode = O_RDONLY;
            break;
        case 'w':
            mode = O_WRONLY | O_CREAT;
            break;
        default:
            luaL_argcheck(L, 0, 2, "invalid mode");
            break;
    }

    fd = open(filename, mode);
    if( fd < 0 )
    {
        perror("open");
        return luaL_error(L, "cannot open %s", filename);
    }
    nBytes = sizeof(handle_t);
    handle = (handle_t*)lua_newuserdata(L, nBytes);
    /* init handle */
    handle->fd = fd;
    handle->mode = mode;
    handle->L = L;
    for( i = 0; i != max_info_size; ++i)
    {
        handle->infos[i] = NULL;
    }

    luaL_getmetatable(L, metatable_name);
    lua_setmetatable(L, -2);
    return 1;
}

static int laio_read(lua_State* L)
{
    size_t nBytes;
    int ret;
    int offset;
    int idx;
    size_t buf_size;
    handle_t* handle;
    info_t* info;

    handle = checkhandle(L);
    luaL_argcheck(L, handle != NULL, 1, "aio_handle expect");
    if( handle->mode != O_RDONLY )
    {
        return luaL_error(L, "file is not open for read");
    }

    luaL_argcheck(L, lua_isnumber(L, 2), 2, "number expect");
    offset = luaL_checkint(L, 2);

    buf_size = luaL_checknumber(L, 3);

    nBytes = sizeof(info_t);

    info = (info_t*)lua_newuserdata(L, nBytes);
    idx = first_slot(&handle->infos[0], max_info_size);
    if( idx == -1 )
    {
        return luaL_error(L, "reach max concurrent async IO limit.");
    }
    info->idx = idx;

    memset(&info->m_aiocb, 0, sizeof(info->m_aiocb) );
    info->buf = malloc( buf_size );
    info->buf_len = buf_size;
    info->m_aiocb.aio_buf = info->buf;
    info->m_aiocb.aio_fildes = handle->fd;
    info->m_aiocb.aio_nbytes = buf_size;
    info->m_aiocb.aio_offset = offset;
    info->m_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    info->m_aiocb.aio_sigevent.sigev_signo = sig_no;
    info->m_aiocb.aio_sigevent.sigev_value.sival_ptr = info;

    info->handle = handle;
    info->L = L;

    ret = aio_read(&info->m_aiocb);
    if( ret < 0 )
    {
        free( info->buf );
        return luaL_error(L, "aio_read failed, ret=%d", ret);
    }
    return lua_yield(L, 0);
}

static int laio_write(lua_State* L)
{
    size_t buf_size, nBytes;
    int idx, ret, offset;
    handle_t* handle;
    info_t* info;
    const char* data;

    handle = checkhandle(L);
    if( handle->mode != (O_WRONLY|O_CREAT) )
    {
        return luaL_error(L, "invalid mode");
    }

    offset = luaL_checkint(L, 2);

    data = lua_tolstring(L, 3, &buf_size);

    nBytes = sizeof(info_t);

    info = (info_t*)lua_newuserdata(L, nBytes);
    idx = first_slot(&handle->infos[0], max_info_size);
    if( idx == -1 )
    {
        return luaL_error(L, "reach max concurrent async IO limit.");
    }
    info->idx = idx;


    memset(&info->m_aiocb, 0, sizeof(info->m_aiocb) );
    info->buf = malloc( buf_size );
    info->buf_len = buf_size;
    memcpy( info->buf, data, buf_size );
    info->m_aiocb.aio_buf = info->buf;
    info->m_aiocb.aio_fildes = handle->fd;
    info->m_aiocb.aio_nbytes = buf_size;
    info->m_aiocb.aio_offset = offset;
    info->m_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    info->m_aiocb.aio_sigevent.sigev_signo = sig_no;
    info->m_aiocb.aio_sigevent.sigev_value.sival_ptr = info;

    info->handle = handle;
    info->L = L;

    ret = aio_write(&info->m_aiocb);
    if( ret < 0 )
    {
        free( info->buf );
        return luaL_error(L, "aio_write failed, ret=%d", ret);
    }
    return lua_yield(L, 0);
}

static int laio_delete_handle(lua_State* L)
{
    handle_t* handle;
    handle = checkhandle(L);
    cleanup_handle(handle);
    return 0;
}

static const struct luaL_Reg aiolib_f[] = {
    {"new", laio_new_handle},
    {NULL, NULL}
};

static const struct luaL_Reg aiolib_m[] = {
    {"read", laio_read},
    {"write", laio_write},
    {"__gc", laio_delete_handle},
    {NULL, NULL}
};

static int register_signal_handle()
{
    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_SIGINFO;
    sig_act.sa_sigaction = aio_completion_handler;
    return sigaction(sig_no, &sig_act, NULL);
}

int luaopen_aio(lua_State* L)
{
    sig_no = SIGRTMIN + 5;
    int ret;
    ret = register_signal_handle();
    if( ret < 0 )
    {
        return luaL_error(L, "set signal handle failed");
    }

    sigemptyset(&sig_blocked);
    sigaddset(&sig_blocked, sig_no);

    luaL_newmetatable(L, metatable_name);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_register(L, NULL, aiolib_m);
    luaL_register(L, "aio", aiolib_f);
    return 1;
}

