#include "../inc/fiber.h"
#include "../inc/log.h"
#include "../inc/mydef.h"
#include "../inc/scheduler.h"
#include "../inc/config.h"
#include <atomic>
#include <stdlib.h>
#include <exception>

#define DEFAULT_STACK_SIZE 1024 * 128

namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");

static fang::ConfigVar<uint32_t>::ptr g_fiber_stack_size
    = fang::Config::Lookup<uint32_t>("fiber.stack.size", 128 * 1024, "fiber satck size");

static std::atomic<uint64_t> s_fiber_id {0};
static std::atomic<uint64_t> s_fiber_count {0};

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_threadfiber = nullptr;

struct StackAllocator {
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp, size_t size) {
        return free(vp);
    }
};

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if (t_fiber) { //当前线程存在执行协程就返回
        return t_fiber->shared_from_this();
    } //没有执行协程，构造一个主协程
    Fiber::ptr main_fiber(new Fiber);
    FANG_ASSERT(t_fiber == main_fiber.get());
    t_threadfiber = main_fiber;
    return t_fiber->shared_from_this();
}

uint64_t Fiber::GetFiberId() {
    if (t_fiber) {
        return t_fiber->getId();
    }
    return 0;
}

uint64_t Fiber::GetTotalFibers() {
    return s_fiber_count;
}

void Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();         //获取线程上的当前协程
    FANG_ASSERT(cur->m_state == EXEC);  //判断协程是否时执行状态
    cur->m_state = READY;               //协程状态设置为ready
    cur->swapOut();                     //让出cpu
}

void Fiber::YieldToHold() {
    //FANG_LOG_DEBUG(g_logger) << "DEBUG3 ---->";
    Fiber::ptr cur = GetThis();
    FANG_ASSERT(cur->m_state == EXEC);

    //cur->m_state = HOLD;
    cur->swapOut();
}

//私有构造函数，用来构造线程的第一个协程
Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);
    int ret = getcontext(&m_ctx);
    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "getContent error, and error=" 
                                << strerror(errno);
    }
    ++s_fiber_count;
    //FANG_LOG_DEBUG(g_logger) << "Fiber::Fiber() called";
}

Fiber::Fiber(std::function<void()> cb, 
        size_t stacksize, bool use_caller)
    :m_id(++s_fiber_id)
    ,m_cb(cb){
    ++s_fiber_count;
    m_stacksize = stacksize ? stacksize : g_fiber_stack_size->getValue();
    m_stack = StackAllocator::Alloc(m_stacksize);
    //m_stack = malloc(128);
    int ret = getcontext(&m_ctx);
    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "getcontext error,"
                            << "and error=" << strerror(errno);
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    if (!use_caller) {
        makecontext(&m_ctx, &Fiber::RootMainFunc, 0);
    } else {
        makecontext(&m_ctx, &Fiber::CallerMainFunc, 0);
    }
}

Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        StackAllocator::Dealloc(m_stack, m_stacksize);
    } else {
        Fiber* cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
}

void Fiber::reset(std::function<void()> cb) {
    FANG_ASSERT(m_stack); //判断当前协程是否有函数执行栈，主协程没有函数栈
    FANG_ASSERT(m_state == TREM
                || m_state == INIT
                || m_state == EXCEPT);  //判断当前协程的状态，如果时ready or exec 就不能进行reset

    m_cb.swap(cb);
    int ret = getcontext(&m_ctx);
    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "getContent error, and error="
                                << strerror(errno);
    }

    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::RootMainFunc, 0);
    m_state = INIT;
}

void Fiber::swapIn() {
    FANG_ASSERT(m_state != EXEC) //判断抢占cpu的协程是否是正在执行的协程
    m_state = EXEC; 
    SetThis(this);
    //将当前协程占有cpu执行任务，执行完后返回线程的主协程
    //FANG_LOG_DEBUG(g_logger) << "DEBUG_2" << "------>" << m_id;
    int ret = swapcontext(&Scheduler::GetMainFiber()->m_ctx, &m_ctx);
    //FANG_LOG_DEBUG(g_logger) << "DEBUG_4" << "------>" << m_id;

    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "swapcontext error, and error="
                                << strerror(errno);
    }
}

void Fiber::swapOut() {
    //FANG_ASSERT(m_state == EXEC) //判断让出cpu的协程是否正在执行的协程
    SetThis(Scheduler::GetMainFiber()); //让出cpu给线程的主协程
    int ret = swapcontext(&m_ctx, &Scheduler::GetMainFiber()->m_ctx);
    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "swapcontext error, and error="
                                << strerror(errno);
    }
}

void Fiber::call() {
    SetThis(this);
    m_state = EXEC;
    // 从当前执行的协程中直接进入非主协程执行
    int ret = swapcontext(&t_threadfiber->m_ctx, &m_ctx);
    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "swapcontext error, and error="
                                << strerror(errno);
    }
}

void Fiber::back() {
    SetThis(t_fiber);
    int ret = swapcontext(&m_ctx, &t_threadfiber->m_ctx);
    if (ret != 0) {
        FANG_LOG_ERROR(g_logger) << "swapcontext error, and error="
                                << strerror(errno);
    }
}

void Fiber::RootMainFunc() {
    Fiber::ptr cur = GetThis();
    FANG_ASSERT(cur);

    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TREM;
    } catch(std::exception& ex) {
        cur->m_state = EXCEPT;
        FANG_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << ", Fiber_id=" << cur->getId();
            //<< std::endl
            //<< fang::BackTraceToString();
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
}

void Fiber::CallerMainFunc(){
    Fiber::ptr cur = GetThis();
    FANG_ASSERT(cur);

    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TREM;
    } catch (std::exception& ex) {
        cur->m_state = EXCEPT;
        FANG_LOG_ERROR(g_logger) << "Fiber Except: " << ex.what()
            << ", Fiber_id=" << cur->getId();
            //<< std::endl
            //<< fang::BackTraceToString(10);
    }
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->back();
}

}
