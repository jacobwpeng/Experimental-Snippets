/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *        Created:  12/08/2013 03:56:25 PM
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  test of aio
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "laio_def.h"

#define checkinfo(L, idx)\
    (info_t*) luaL_checkudata(L, idx, info_metatable_name)

#define checkhandle(L, idx)\
    (handle_t*) luaL_checkudata(L, idx, handle_metatable_name)

typedef void (*signal_handle_t)(int, siginfo_t*, void*);

static volatile sig_atomic_t ready = 0;
static sigset_t sig_blocked;
static int aio_signo;
static const char * handle_metatable_name = "l_aio_handle_m";
static const char * info_metatable_name = "l_aio_info_m";

static const int AIO_READ = O_RDONLY;
static const int AIO_WRITE = O_WRONLY | O_CREAT;

static void aio_completion_handler(int signo, siginfo_t* sig_info, void* ctx)
{
    info_t * info;
    handle_t * handle;

    info = (info_t*)sig_info->si_value.sival_ptr;
    info->aio_ret = aio_error(&info->m_aiocb);
    if( info->aio_ret == 0 ) info->ret = aio_return(&info->m_aiocb);
    info->done = 1;
}

void register_signal_handle(int signo, signal_handle_t handle)
{
    struct sigaction sig_act;
    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_SIGINFO;
    sig_act.sa_sigaction = aio_completion_handler;
    if (sigaction(signo, &sig_act, NULL) != 0)
    {
        perror("sigaction");
    }
}

static int laio_newhandle(lua_State* L)
{
    int fd;
    int mode;
    size_t len;
    const char * filename;
    const char * mode_str;
    handle_t * handle;

    filename = lua_tolstring(L, 1, &len);
    luaL_argcheck(L, len != 0u, 1, "invalid name");

    mode_str = lua_tolstring(L, 2, &len);
    luaL_argcheck(L, len == 1u, 2, "invalid mode");

    switch(mode_str[0])
    {
        case 'r':
            mode = AIO_READ;
            break;
        case 'w':
            mode = AIO_WRITE;
            break;
        default:
            mode = -1;                          /* avoid warnings */
            luaL_argcheck(L, 0, 2, "invalid mode");
            break;
    }

    fd = open(filename, mode, 0666);
    if( fd < 0 )
    {
        perror("open");
        return luaL_error(L, "cannot open %s", filename);
    }
    handle = (handle_t*)lua_newuserdata(L, sizeof(handle_t));
    handle->L = L;
    handle->fd = fd;
    handle->mode = mode;
    handle->gc = 0;
    memset( handle->infos, 0x0, sizeof(handle->infos) );

    luaL_getmetatable(L, handle_metatable_name);
    lua_setmetatable(L, -2);

    return 1;
}

int laio_read(lua_State* L)
{
    int ret;
    off_t offset;
    size_t nbytes;
    info_t * info;
    handle_t * handle;

    handle = checkhandle(L, 1);
    if( handle->mode != AIO_READ )
    {
        return luaL_error(L, "file is not open for read");
    }

    luaL_argcheck(L, lua_isnumber(L, 2), 2, "number expect");
    offset = luaL_checkint(L, 2);

    luaL_argcheck(L, lua_isnumber(L, 3), 3, "number expect");
    nbytes = luaL_checknumber(L, 3);

    info = (info_t*) lua_newuserdata(L, sizeof(info_t));
    info->idx = first_slot(&handle->infos[0], max_info_size);
    if (info->idx == -1)
    {
        return luaL_error(L, "reach max concurrent async IO limit.");
    }

    memset(&info->m_aiocb, 0, sizeof(info->m_aiocb) );
    info->ret = 0;
    info->aio_ret = 0;
    /* TODO : maybe use luaL_Buffer */
    info->buf = malloc(nbytes);
    info->buf_len = nbytes;
    info->handle = handle;
    info->L = L;
    info->done = 0;
    info->gc = 0;

    info->m_aiocb.aio_buf = info->buf;
    info->m_aiocb.aio_fildes = handle->fd;
    info->m_aiocb.aio_nbytes = nbytes;
    info->m_aiocb.aio_offset = offset;
    info->m_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    info->m_aiocb.aio_sigevent.sigev_signo = aio_signo;
    info->m_aiocb.aio_sigevent.sigev_value.sival_ptr = info;

    ret = aio_read(&info->m_aiocb);
    if (ret < 0)
    {
        free(info->buf);
        perror("aio_read");
        return luaL_error(L, "aio_read failed, ret=%d", ret);
    }

    luaL_getmetatable(L, info_metatable_name);
    lua_setmetatable(L, -2);

    return 1;
}

int laio_write(lua_State* L)
{
    int ret;
    off_t offset;
    size_t nbytes;
    info_t * info;
    handle_t * handle;
    const char * buf;

    handle = checkhandle(L, 1);
    if( handle->mode != AIO_WRITE )
    {
        return luaL_error(L, "file is not open for write");
    }

    luaL_argcheck(L, lua_isnumber(L, 2), 2, "number expect");
    offset = luaL_checkint(L, 2);

    luaL_argcheck(L, lua_isstring(L, 3), 3, "string expect");
    buf = lua_tolstring(L, 3, &nbytes);

    info = (info_t*) lua_newuserdata(L, sizeof(info_t));
    info->idx = first_slot(&handle->infos[0], max_info_size);
    if (info->idx == -1)
    {
        return luaL_error(L, "reach max concurrent async IO limit.");
    }

    info->ret = 0;
    info->aio_ret = 0;
    info->buf = malloc(nbytes);
    info->buf_len = nbytes;
    memcpy(info->buf, buf, nbytes);
    info->handle = handle;
    info->L = L;
    info->done = 0;
    info->gc = 0;

    memset(&info->m_aiocb, 0x0, sizeof(info->m_aiocb) );
    info->m_aiocb.aio_buf = info->buf;
    info->m_aiocb.aio_fildes = handle->fd;
    info->m_aiocb.aio_nbytes = nbytes;
    info->m_aiocb.aio_offset = offset;
    info->m_aiocb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    info->m_aiocb.aio_sigevent.sigev_signo = aio_signo;
    info->m_aiocb.aio_sigevent.sigev_value.sival_ptr = info;

    ret = aio_write(&info->m_aiocb);
    if (ret < 0)
    {
        free(info->buf);
        perror("aio_write");
        return luaL_error(L, "aio_write failed, ret=%d, errno=%d", ret, errno);
    }

    luaL_getmetatable(L, info_metatable_name);
    lua_setmetatable(L, -2);

    return 1;
}

int laio_wait(lua_State* L)
{
    sigset_t empty;
    int table_idx = 1;
    int all_done;
    int state_top;
    info_t * info;

    luaL_argcheck(L, lua_istable(L, table_idx), table_idx, "table expect");

    sigemptyset(&empty);
    sigprocmask(SIG_BLOCK, &sig_blocked, NULL);
    while (1)
    {
        state_top = lua_gettop(L);
        assert( state_top == 1 );
        all_done = 1;
        lua_pushnil(L);                         /* first key */
        while (lua_next(L, table_idx) != 0 )
        {
            info = checkinfo(L, -1);
            if (info->done == 0)
            {
                all_done = 0;
                /* wait for new aio signal */
                sigsuspend(&empty);
                if (errno != EINTR) perror("sigsuspend");
                /* remove both key and value */
                lua_pop(L, 2);
                /* and break for next iteration */
                break;
            }
            else
            {
                lua_pop(L, 1);
            }
        }

        if (all_done) break;
    }
    sigprocmask(SIG_UNBLOCK, &sig_blocked, NULL);
    return 0;
}

//int laio_waitone(lua_State* L)
//{
//    sigset_t empty;
//    int table_idx = 1;
//    int all_done;
//    int state_top;
//    info_t * info;
//
//    luaL_argcheck(L, lua_istable(L, table_idx), table_idx, "table expect");
//
//    sigemptyset(&empty);
//    sigprocmask(SIG_BLOCK, &sig_blocked, NULL);
//    while (1)
//    {
//        state_top = lua_gettop(L);
//        assert( state_top == 1 );
//        all_done = 1;
//        lua_pushnil(L);                         /* first key */
//        while (lua_next(L, table_idx) != 0 )
//        {
//            info = checkinfo(L, -1);
//            if (info->done == 0)
//            {
//                all_done = 0;
//                /* wait for new aio signal */
//                sigsuspend(&empty);
//                if (errno != EINTR) perror("sigsuspend");
//                /* remove both key and value */
//                lua_pop(L, 2);
//                /* and break for next iteration */
//                break;
//            }
//            else
//            {
//                lua_pop(L, 1);
//            }
//        }
//
//        if (all_done) break;
//    }
//    sigprocmask(SIG_UNBLOCK, &sig_blocked, NULL);
//    return 0;
//}

int laio_retrieve(lua_State* L)
{
    int ret;
    info_t * info;

    info = checkinfo(L, 1);
    assert( info->handle->mode == AIO_READ );
    assert( info->gc == 0 );
    assert( info->done == 1 );
    assert( info->aio_ret == 0 );

    lua_pushlstring(L, info->buf, info->ret);
    return 1;
}

int laio_len(lua_State* L)
{
    int ret;
    struct stat sb;
    handle_t * handle;

    handle = checkhandle(L, 1);
    ret = fstat(handle->fd, &sb);
    if (ret != 0 ) perror("fstat");

    lua_pushnumber(L, sb.st_size);
    return 1;
}

int laio_deletehandle(lua_State* L)
{
    handle_t* handle;
    handle = checkhandle(L, 1);
    cleanup_handle(handle);
    return 0;
}

int laio_deleteinfo(lua_State* L)
{
    info_t * info;
    info = checkinfo(L, 1);
    cleanup_info(info);
    return 0;
}

static const struct luaL_Reg aiolib_f[] = {
    {"new", laio_newhandle},
    {"wait", laio_wait},
    {NULL, NULL}
};

static const struct luaL_Reg aiolib_m[] = {
    {"read", laio_read},
    {"write", laio_write},
    {"len", laio_len},
    {"__gc", laio_deletehandle},
    {NULL, NULL}
};

static const struct luaL_Reg aiolib_info_func[] = {
    {"retrieve", laio_retrieve},
    {"destroy", laio_deleteinfo},
    {"__gc", laio_deleteinfo},
    {NULL, NULL}
};

int luaopen_aio(lua_State* L)
{
    aio_signo = SIGRTMIN + 5;
    register_signal_handle(aio_signo, aio_completion_handler);

    sigemptyset(&sig_blocked);
    sigaddset(&sig_blocked, aio_signo);

    luaL_newmetatable(L, handle_metatable_name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, aiolib_m);
    luaL_register(L, "aio", aiolib_f);

    luaL_newmetatable(L, info_metatable_name);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, aiolib_info_func);


    return 2;
}
