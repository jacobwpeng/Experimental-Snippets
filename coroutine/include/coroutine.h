/*
 * =====================================================================================
 *
 *       Filename:  coroutine.h
 *        Created:  08/06/14 11:24:05
 *         Author:  peng wang
 *          Email:  pw2191195@gmail.com
 *    Description:  linux specific coroutine
 *
 * =====================================================================================
 */

#ifndef  __COROUTINE_H__
#define  __COROUTINE_H__

#include <ucontext.h>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>

class Coroutine : boost::noncopyable
{
    public:
        typedef boost::any Context;
        typedef boost::function<Context(Coroutine*)> Routine;
        enum Status
        {
            kSuspended = 1,
            kRunning = 2,
            kDead = 3
        };

        explicit Coroutine(Routine routine, size_t stack_size = 1 << 21);

        Context Yield(Context ctx = Context());
        Context Resume(Context ctx = Context());
        Status status() const;
        Context ctx() const;

    private:
        static void CoroutineFunction(Coroutine * co);

    private:
        const size_t kStackSize;
        bool in_routine_;
        Context ctx_;
        Routine routine_;
        Status status_;
        boost::scoped_array<char> stack_;
        ucontext_t coroutine_done_point_;
        ucontext_t yield_recovery_point_;
        ucontext_t coroutine_context_;
};

#endif   /* ----- #ifndef __COROUTINE_H__  ----- */
