#ifndef __FANG_FIBER_H__
#define __FANG_FIBER_H__

#include <memory>
#include <functional>
#include <ucontext.h>
#include <string.h>
namespace fang {

class Fiber : public std::enable_shared_from_this<Fiber> {

    public:
        typedef std::shared_ptr<Fiber> ptr;

        enum State {//协程状态
            INIT,   //初始状态
            HOLD,   //暂停状态
            EXEC,   //执行状态
            TREM,   //结束状态
            READY,  //可执行状态
            EXCEPT  //异常状态
        };

    public:
        Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
        ~Fiber();

        uint64_t getId() const { return m_id; }
        State getState() const { return m_state; }

        void reset(std::function<void()> cb);   //重置协程的执行函数

        void swapIn();      //将协程切入为执行协程
        void swapOut();     //将协程切出到后台

        void call();
        void back();

    public:
        static void SetThis(Fiber* f);
        static Fiber::ptr GetThis();

        static uint64_t GetFiberId();       //返回当前协程的id
        static uint64_t GetTotalFibers();   //返回当前协程的总数


        static void YieldToReady();     //将当前协程切换到后台挂起，并设置为可执行状态(ready)
        static void YieldToHold();      //将当前协程切换到后台，并设置为HOLD状态

        static void RootMainFunc();     //协程的执行函数，执行完后返回到线程的主协程上去
        static void CallerMainFunc();   //协程的执行函数，执行完后返回到线程的调度协程上去
    private:
        Fiber();

    private:
        uint64_t m_id;              //协程id
        uint32_t m_stacksize;       //协程栈大小
        State m_state;           //协程状态
        ucontext_t m_ctx;               //协程上下文
        void* m_stack;        //协程运行栈指针
        std::function<void()> m_cb;     //协程运行函数
};




}



#endif
