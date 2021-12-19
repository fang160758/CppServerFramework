#pragma once
#include <vector>
#include <list>
#include <memory>
#include <functional>
#include <string.h>
#include <atomic>
#include "mutex.h"
#include "thread.h"
#include "fiber.h"

namespace fang {

class Scheduler {

    public:
        typedef std::shared_ptr<Scheduler> ptr;
        typedef Mutex MutexType;
    
    public:
        Scheduler(size_t threads = 1, bool use_caller = true, 
                const std::string& name = "");

        ~Scheduler();

        const std::string& getName() const { return m_name; }
        void start();
        void stop();

        void switchTo(int thread);
        std::ostream& dump(std::ostream& os);


        static Scheduler* GetThis();
        static Fiber* GetMainFiber();

        template<typename FiberOrCb>
        void schedul(FiberOrCb fc, int thread = -1) {
            bool need_tickle = false;
            {
                MutexType::Lock lock(m_mutex);
                need_tickle = schedulNoLock(fc, thread);
            }
            if (need_tickle) {
                tickle();
            }
        }

        template<typename InputIterator>
        void schedul(InputIterator begin, InputIterator end) {
            bool need_tickle = false;
            {
                MutexType::Lock lock(m_mutex);
                while(begin != end) {
                    need_tickle = schedulNoLock(&*begin, -1) || need_tickle;
                    ++begin;
                }
            }
            if (need_tickle) {
                tickle();
            }
        }



    protected:
        virtual void tickle();
        virtual void stopping();
        virtual void idle();

        void run();
        void setThis();
        bool hasIdleThreads() {  return m_idleThreadCount > 0; }

    private:
        template<typename FiberOrCb>
        bool schedulNoLock(FiberOrCb fc, int thread) {
            bool need_tickle = m_fiberList.empty();
            FiberAndThread task(fc, thread);
            if (task.cb || task.fiber) {
                m_fiberList.push_back(task);
            }
            return need_tickle;
        }

    private:
        struct FiberAndThread {
            Fiber::ptr fiber;
            std::function<void()> cb;
            int threadId;

            FiberAndThread(Fiber::ptr f, int thr)
                :fiber(f)
                ,threadId(thr) {}

            FiberAndThread(Fiber::ptr* f, int thr)
                :threadId(thr) { fiber.swap(*f); }

            FiberAndThread(std::function<void()> _cb, int thr)
                :cb(_cb)
                ,threadId(thr) {}

            FiberAndThread(std::function<void()> *_cb, int thr)
                :threadId(thr) { cb.swap(*_cb); }

            FiberAndThread()
                :threadId(-1) {}

            void reset() {
                fiber = nullptr;
                cb = nullptr;
                threadId = -1;
            }
        };

    private:
        MutexType m_mutex;
        std::vector<Thread::ptr> m_threadPool;
        std::list<FiberAndThread> m_fiberList;
        Fiber::ptr m_rootFiber;
        std::string m_name;

    protected:
        std::vector<int> m_threadIds;
        size_t m_threadCount = 0;
        std::atomic<size_t> m_activeThreadCount = {0};
        std::atomic<size_t> m_idleThreadCount = {0};
        bool m_stopping = true;
        bool m_autoStop = false;
        int m_rootThread = 0;
};


}
