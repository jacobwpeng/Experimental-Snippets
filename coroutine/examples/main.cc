/*
 * =====================================================================================
 *
 *       Filename:  main.cc
 *        Created:  08/05/14 17:29:30
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */

#include <cassert>
#include <iostream>
#include "coroutine.h"

using namespace std;

int Producer(Coroutine * co)
{
    for (int i = 0; i != 10; ++i)
    {
        cout << "producer" << '\n';
        co->Yield(i);
    }
    return 0;
}

int Consumer(Coroutine * co)
{
    Coroutine::Context ctx = co->ctx();
    do
    {
        int val = boost::any_cast<int>(ctx);
        std::cout << "consumer -> " << val << '\n';
        ctx = co->Yield();
    }while (not ctx.empty());
    return 0;
}

int main()
{
    Coroutine producer(Producer);
    Coroutine consumer(Consumer);
    Coroutine::Context ctx = producer.Resume();
    do
    {
        consumer.Resume(ctx);
        ctx = producer.Resume();
    }
    while (producer.status() != Coroutine::kDead);
    assert (consumer.status() == Coroutine::kSuspended);
    return 0;
}
