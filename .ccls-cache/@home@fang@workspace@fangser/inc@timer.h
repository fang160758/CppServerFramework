#ifndef __FANG_TIMER_H__
#define __FANG_TIMER_H__

#include <memory>
#include <vector>
#include <set>
#include <functional>
#include <stdlib.h>
#include "mutex.h"

namespace fang {
    
    class TimerManager;

    //定时器
    class Timer : public std::enable_shared_from_this<Timer> {
        public:
            friend class TimerManager;
            typedef std::shared_ptr<Timer> ptr;//定时器的智能指针


        public:
            Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager * tmgr);

            Timer(uint64_t next);
            

            bool cancel(); //取消定时任务
            bool refresh(); //刷新设置定时器的时间
            bool reset(uint64_t ms, bool from_now); //重置定时器执行时间

        private:
            bool m_recurring = false; //是否循环定时器
            uint64_t m_ms = 0; //执行周期
            uint64_t m_next = 0; //精确地执行时间
            std::function<void()> m_cb; //回调函数
            TimerManager *m_TMgr = nullptr;

        private:
            struct Comparator {

                bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
            };
    };


    //定时器管理类
    class TimerManager {
        public:
            friend class Timer;

        public:
            TimerManager();
            virtual ~TimerManager(){}

            //添加一个定时器
            Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);
            
            //添加条件定时器
            Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb,
                    std::weak_ptr<void> weak_cond, bool recurring = false);

            //获取最近的定时器的时间差
            uint64_t getNextTimer();

            //获取需要执行处理函数的定时器的处理函数列表
            void listExpiredCb(std::vector<std::function<void()> > &cbs);

            //是否有定时器
            bool hasTimer();


        protected:
            virtual void onTimerInsertedAtFront() = 0;
            void addTimer(Timer::ptr val, RWMutex::WrLock& lock);
            
        private:
            //检测服务器时间是否被调后
            bool detectClockRollover(uint64_t now_ms);


        private:
           RWMutex m_mutex;//一个全局读写锁
           std::set<Timer::ptr, Timer::Comparator> m_timers;//一个所有定时器的容器
           bool m_tickled = false; //是否触发
           uint64_t m_previouseTime = 0; //上次执行时间
    };


}

#endif
