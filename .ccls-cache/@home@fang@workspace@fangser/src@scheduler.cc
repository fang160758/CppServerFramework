#include "../inc/scheduler.h"
#include "../inc/log.h"
#include "../inc/hook.h"
#include "../inc/macro.h"
#include "../inc/helpc.h"
namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system"); 

static thread_local Scheduler* t_scheruler = nullptr;
static thread_local Fiber* t_scheruler_fiber = nullptr;

Scheduler* Scheduler::GetThis() {
    return t_scheruler;
}

Fiber* Scheduler::GetMainFiber() {
    return t_scheruler_fiber;
}

Scheduler::Scheduler(size_t threads, bool use_caller, 
        const std::string& name)
    :m_name(name) {
    FANG_ASSERT(threads > 0);
    if (use_caller) {
        fang::Fiber::GetThis();
        --threads;

        t_scheruler = this;
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        fang::Thread::SetName(m_name);
        t_scheruler_fiber = m_rootFiber.get();
        m_rootThread = fang::GetThreadId();
        m_threadIds.push_back(m_rootThread);
    } else {
        m_rootThread = -1;
    }
    m_threadCount = threads;
}

Scheduler::~Scheduler() {
    FANG_ASSERT(m_stopping);
    if (GetThis() == this) {
        t_scheruler = nullptr;
    }
}
void Scheduler::start() {
    MutexType::Lock lock(m_mutex);

    if (!m_stopping) {
        return ;
    }

    m_stopping = false;

    FANG_ASSERT(m_threadPool.empty()); //新的调度器的线程池应该为零

    m_threadPool.resize(m_threadCount); 

    for (size_t i = 0; i < m_threadCount; i++) {    //创建线程池
        m_threadPool[i].reset(new Thread(std::bind(&Scheduler::run, this), 
                    m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threadPool[i]->getId());
    }
    lock.unlock();
}

void Scheduler::stop() {
    m_autoStop = true;
    if (m_rootFiber 
            && m_threadCount == 0
            && (m_rootFiber->getState() == Fiber::TREM
                || m_rootFiber->getState() == Fiber::INIT)) {
        FANG_LOG_INFO(g_logger) << "stopped";
        m_stopping = true;
        if (stopping())
            return;
    }
    if(m_rootThread != -1) {
        FANG_ASSERT(GetThis() == this);
    } 
    else {
        FANG_ASSERT(GetThis() != this);
    } 

    m_stopping = true;
    for (size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if (m_rootFiber) {
        tickle();
    }

    if (m_rootFiber) {
        if (!stopping()) {
            m_rootFiber->call();
        }
    }
    std::vector<Thread::ptr> thrs;
    {
        MutexType::Lock lock(m_mutex);
        thrs.swap(m_threadPool);
    }
    for (auto& i : thrs) {
        i->join();
    }

}
void Scheduler::switchTo(int thread) {
    FANG_ASSERT(Scheduler::GetThis() != nullptr);
    if (Scheduler::GetThis() == this) {
        if (thread == -1 || thread == fang::GetThreadId()) {
            return;
        }
    }
    schedul(Fiber::GetThis(), thread);
    Fiber::YieldToHold();
}

std::ostream& Scheduler::dump(std::ostream& os) {
    os << "[Scheduler name=" << m_name
       << ", size=" << m_threadCount
       << ", active_count=" << m_activeThreadCount
       << ", idle_thread=" << m_idleThreadCount
       << ", stopping=" << m_stopping
       << "]" << std::endl << "   ";
    for (size_t i = 0; i < m_threadPool.size(); i++) {
        if (i) {
            os << ", ";
        }
        os << m_threadPool[i];
    }
    return os;
}

void Scheduler::tickle() {
    FANG_LOG_INFO(g_logger) << "have task to run";
}

bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);

    //FANG_LOG_DEBUG(g_logger) << "run stopping ";
    return m_autoStop && m_stopping 
        && m_fiberList.empty() && m_activeThreadCount == 0;
}

void Scheduler::idle() {
    FANG_LOG_INFO(g_logger) << "idle";
    while(!stopping()) { 
        fang::Fiber::YieldToHold();
    }
}
void Scheduler::setThis() {
    t_scheruler = this;
}

void Scheduler::run() {
    FANG_LOG_DEBUG(g_logger) << "DEBUG ---->"
                    << fang::Thread::GetThis()->getId()
                    << "-----" << fang::Thread::GetName();
    set_hook_enable(true);
    setThis();
    if (fang::GetThreadId() != m_rootThread) {
        t_scheruler_fiber = Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;
    FiberAndThread ft;
    while(true){
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            MutexType::Lock lock(m_mutex);
            auto it = m_fiberList.begin();
            while(it != m_fiberList.end()) {
                if (it->threadId != -1 && it->threadId != fang::GetThreadId()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                FANG_ASSERT(it->fiber || it->cb);

                if (it->fiber && it->fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                ft = *it;
                m_fiberList.erase(it++);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
            tickle_me |= it != m_fiberList.end();
        }
        if (tickle_me) {
            tickle();
        }
        if (ft.fiber && (ft.fiber->getState() != Fiber::TREM
                    && ft.fiber->getState() != Fiber::EXCEPT)) {
            ft.fiber->swapIn();
            --m_activeThreadCount;
            if (ft.fiber->getState() == Fiber::READY) {
                schedul(ft.fiber);
            } else if (ft.fiber->getState() != Fiber::TREM
                    && ft.fiber->getState() != Fiber::EXCEPT) {
                ft.fiber->m_state = Fiber::HOLD;
            }
            ft.reset();
        } else if (ft.cb) {
            if (cb_fiber) {
                cb_fiber->reset(ft.cb);
            } else {
                cb_fiber.reset(new Fiber(ft.cb));
            }
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if (cb_fiber->getState() == Fiber::READY) {
                schedul(cb_fiber);
                cb_fiber.reset();
            } else if (cb_fiber->getState() == Fiber::EXCEPT 
                    || cb_fiber->getState() == Fiber::TREM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber->m_state = Fiber::HOLD;
                cb_fiber.reset();
            }
        } else {
            if (is_active) {
                --m_activeThreadCount;
                continue;
            }
            if (idle_fiber->getState() == Fiber::TREM) {
                FANG_LOG_INFO(g_logger) << "idle fiber trem";
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if (idle_fiber->getState() != Fiber::TREM
                    && idle_fiber->getState() != Fiber::EXCEPT) {
                idle_fiber->m_state = Fiber::HOLD;
            }
        }
    }
}

}
