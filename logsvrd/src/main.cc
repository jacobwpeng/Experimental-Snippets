/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  06/04/14 11:49:56
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cstdio>
#include "logsvrd.h"

using namespace std;

int main(int argc, char * argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s conf\n", argv[0]);
        return EXIT_FAILURE;
    }
    LogServer s;
    int ret = s.Init(argv[1]);
    if (ret < 0)
    {
        fprintf(stderr, "LogServer Init Failed, ret: %d\n", ret);
        return EXIT_FAILURE;
    }
    s.Run();
    return EXIT_SUCCESS;
}
