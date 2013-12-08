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

#include <stdlib.h>
#include <stdio.h>
#include <aio.h>
#include <fcntl.h>
#include <memory.h>
#include <errno.h>
#include <signal.h>

typedef struct { struct aiocb m_aiocb; int ret; } info_t;

void aio_completion_handler(int signo, siginfo_t* info, void* ctx)
{
    int ret;
    struct aiocb* req;
    info_t* pInfo = NULL;
    if( info->si_signo != SIGIO ) return;

    pInfo = (info_t*) info->si_value.sival_ptr;

    req = &(pInfo->m_aiocb);

    if( aio_error(req) != 0 ) perror("aio_error");

    ret = aio_return(req);
    
    pInfo->ret = 1;

    return;
}

int main(int argc, char* argv[])
{
    if( argc != 2 )
    {
        printf("Usage : %s file\n", argv[0]);
        return -1;
    }

    const unsigned BUFSIZE = 1 << 10;
    int ret, fd;
    info_t* pInfo = malloc(sizeof(info_t));
    pInfo->ret = 0;
    struct aiocb* ptr = &(pInfo->m_aiocb);
    struct sigaction sig_act;

    sigemptyset(&sig_act.sa_mask);
    sig_act.sa_flags = SA_SIGINFO;
    sig_act.sa_sigaction = aio_completion_handler;

    fd = open(argv[1], O_RDONLY);
    if( fd < 0 ) perror("open");

    memset(ptr, 0, sizeof(struct aiocb) );

    ptr->aio_buf = malloc( BUFSIZE + 1 );
    if( NULL == ptr->aio_buf ) perror("malloc");

    ptr->aio_fildes = fd;
    ptr->aio_nbytes = BUFSIZE;
    ptr->aio_offset = 0;
    ptr->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    ptr->aio_sigevent.sigev_signo = SIGIO;
    ptr->aio_sigevent.sigev_value.sival_ptr = ptr;

    ret = sigaction(SIGIO, &sig_act, NULL);
    if( ret < 0 ) perror("sigaction");

    ret = aio_read(ptr);
    if( ret < 0 ) perror("aio_read");
    int idx = 0;
    while(pInfo->ret == 0) { printf("%d\n", ++idx); }

    return 0;
}
