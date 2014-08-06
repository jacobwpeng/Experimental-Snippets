/*
 * =====================================================================================
 *
 *       Filename:  coroutine.cc
 *        Created:  08/06/14 11:38:28
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  
 *
 * =====================================================================================
 */
#include "coroutine.h"

Coroutine::Coroutine(Routine routine, size_t stack_size)
    :kStackSize(stack_size), in_routine_(false), routine_(routine), status_(kSuspended), stack_(new char[stack_size])
{

}

Coroutine::Context Coroutine::Yield(Coroutine::Context ctx)
{
    assert (status_ == kRunning);
    status_ = kSuspended;
    ctx_ = ctx;
    swapcontext(&coroutine_context_, &yield_recovery_point_);
    return ctx_;
}

Coroutine::Context Coroutine::Resume(Coroutine::Context ctx)
{
    assert (status_ == kSuspended);
    if (not in_routine_)
    {
        //initial resume
        getcontext(&coroutine_done_point_);
    }

    if (not in_routine_)
    {
        //initial resume
        getcontext(&coroutine_context_);
        coroutine_context_.uc_link = &coroutine_done_point_;
        coroutine_context_.uc_stack.ss_sp = stack_.get();
        coroutine_context_.uc_stack.ss_size = kStackSize;

        makecontext(&coroutine_context_, (void (*)(void)) Coroutine::CoroutineFunction, 1, this);
        in_routine_ = true;
        status_ = kRunning;
        ctx_ = ctx;
        swapcontext(&yield_recovery_point_, &coroutine_context_);
        return ctx_;
    }
    else if (status_ == kSuspended)
    {
        //later resume
        status_ = kRunning;
        ctx_ = ctx;
        swapcontext(&yield_recovery_point_, &coroutine_context_);
        return ctx_;
    }
    else
    {
        //coroutine function is done
        assert (status_ == kRunning);
        assert (in_routine_);
        status_ = kDead;
        return ctx_;
    }
}

Coroutine::Status Coroutine::status() const { return status_; }
Coroutine::Context Coroutine::ctx() const { return ctx_; }

void Coroutine::CoroutineFunction(Coroutine * co)
{
    co->ctx_ = co->routine_(co);
}
