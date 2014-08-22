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
#include <glog/logging.h>

int main(int argc, char * argv[])
{
    (void)argc;
    google::InitGoogleLogging(argv[0]);
    ArenaServer server("127.0.0.1", 6789);
    server.Start();
    return 0;
}
