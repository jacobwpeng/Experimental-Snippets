/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  08/22/14 16:30:37
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include "arena_server.h"
#include <cstdio>
#include <glog/logging.h>

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        ::fprintf(stderr, "Usage : %s conf\n", argv[0]);
        return -1;
    }
    ::daemon(0, 0);
    google::InitGoogleLogging(argv[0]);
    ArenaServer server("0.0.0.0", 6789);
    int ret = server.Init(argv[1]);
    if (ret != 0)
    {
        LOG(ERROR) << "Init failed, ret = " << ret;
        return ret;
    }
    else
    {
        server.Start();
        return 0;
    }
}
