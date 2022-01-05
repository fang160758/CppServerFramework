#include "../inc/timer.h"
#include "../inc/helpc.h"
#include <sys/time.h>


namespace fang {


bool Timer::Comparator::operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const {
    if (!lhs && !rhs) {
        return false;
    }
    if (!lhs) {
        return true;
    }
    if (!rhs) {
        return false;
    }
    if (lhs->m_next < rhs->m_next) {
        return true;
    } else if (lhs->m_next > rhs->m_next) {
        return false;
    } else {
        return lhs.get() < rhs.get();
    }
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager * tmgr)
    : m_recurring(recurring)
    , m_ms(ms)
    , m_cb(cb)
    , m_TMgr(tmgr) {
        m_next = GetCurrentMS() + m_ms;//加上设定时间   
      }

Timer::Timer(uint64_t next) : m_next(next) {};


bool Timer::cancel() {
    //开启写锁
    RWMutex::WrLock lock(m_TMgr->m_mutex);

    if (m_cb) {//该定时器是否设置了处理函数
        m_cb = nullptr;
        auto it = m_TMgr->m_timers.find(shared_from_this());//在容器中查找
        m_TMgr->m_timers.erase(it);//删除该定时器
        return true;
    }
    return false;
}

bool Timer::refresh() {
    //开启读锁
    RWMutex::WrLock lock(m_TMgr->m_mutex);
    if (!m_cb) {
        return false;
    }

    auto it = m_TMgr->m_timers.find(shared_from_this());
    if (it == m_TMgr->m_timers.end()) {
        return false;
    }

    m_TMgr->m_timers.erase(it);
    m_next = GetCurrentMS() + m_ms;
    m_TMgr->m_timers.insert(shared_from_this());
    return true;
}

bool Timer::reset(uint64_t ms, bool from_now) {
    if (ms == m_ms && !from_now) {
        return true;
    }
    
    RWMutex::WrLock lock(m_TMgr->m_mutex);
    if (!m_cb) {
        return false;
    }

    auto it = m_TMgr->m_timers.find(shared_from_this());
    if (it == m_TMgr->m_timers.end()) {
        return false;
    }

    m_TMgr->m_timers.erase(it);
    uint64_t start = 0;
    if (from_now) {
        start = GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = m_ms + start;
    m_TMgr->m_timers.insert(shared_from_this());
    return true;
}

TimerManager::TimerManager() {
    m_previouseTime = GetCurrentMS();
}


//添加一个定时器
Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, 
        bool recurring) {
    Timer::ptr timer(new Timer(ms, cb, recurring, this));
    RWMutex::WrLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;    
}
        

static void OnTimer(std::weak_ptr<void>weak_cond, std::function<void()> cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp) {
        cb();
    }
}
//添加条件定时器
Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb,
        std::weak_ptr<void> weak_cond, bool recurring) {
    return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
}

//获取最近的定时器的时间差
uint64_t TimerManager::getNextTimer() {
    RWMutex::RdLock lock(m_mutex);
    m_tickled = false;
    if (m_timers.empty()) {
        return ~0;
    }

    const Timer::ptr& next = *m_timers.begin();
    uint64_t now_ms = GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return  next->m_next - now_ms;
    }
}

//获取需要执行处理函数的定时器的处理函数列表
void TimerManager::listExpiredCb(std::vector<std::function<void()> > &cbs) {
    uint64_t now_ms = GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutex::RdLock lock(m_mutex);
        if (m_timers.empty()) {
            return ;
        }
    }

    RWMutex::WrLock lock(m_mutex);
    if (m_timers.empty()) {
        return ;
    }

    bool rollover = detectClockRollover(now_ms);
    if (!rollover && ((*m_timers.begin())->m_next > now_ms)) {
        return ;
    }

    Timer::ptr now_timer(new Timer(now_ms));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now_ms) {
        ++it;
    }

    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);

    cbs.reserve(expired.size());

    for (auto& timer : expired) {
        cbs.push_back(timer->m_cb);
        if (timer->m_recurring) {
            timer->m_next = now_ms + timer->m_ms;
            m_timers.insert(timer);
        } else {
            timer->m_cb = nullptr;
        }
    }
}

void TimerManager::addTimer(Timer::ptr val, RWMutex::WrLock& lock) {
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if (at_front) {
        m_tickled = true;
    }
    lock.unlock();

    if (at_front) {
        //tickle()
    }
}
//是否有定时器
bool TimerManager::hasTimer() {
    RWMutex::RdLock lock(m_mutex);
    return m_timers.empty();
}

//检测服务器时间是否被调后
bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_previouseTime &&
            now_ms < (m_previouseTime -60 * 60 * 1000)) {
        rollover = true;
    }
    m_previouseTime = now_ms;
    return rollover;
}



}
