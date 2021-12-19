#include "../inc/iomanager.h"
#include "../inc/log.h"
#include "../inc/macro.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>


#define MAX_EVENtS 256; 
namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");


IoManager* IoManager::GetThis() {
    return dynamic_cast<IoManager*>(Scheduler::GetThis());
}

void IoManager::tickle() {
    if(!hasIdleThreads()) {
        return ;
    }
    int ret = write(m_tickleFds[1], "T", 1);

}

bool IoManager::stopping(uint64_t& timeout) {
    timeout = getNextTimer();
    return timeout == ~0ull
        && m_pendingEventCount == 0
        && Scheduler::stopping();
}

bool IoManager::stopping() {
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IoManager::idle() {
    FANG_LOG_DEBUG(g_logger) << "idle";
    epoll_event* events = new epoll_event[MAX_EVENtS]();
    std::shared_ptr<epoll_event> share_events(events, [](epoll_event* ptr){
            delete[] ptr;
    });

    while (true) {
        uint64_t next_timeout = 0;
        if (stopping(next_timeout)) {
            FANG_LOG_INFO(g_logger) << "name=" << getName()
                                    << " idle stopping exit";
            break;
        }

        int ret = 0;
        do {
            static const int MAX_TIMOUT = 3000;
            if(next_timeout != ~0ull) {
                next_timeout = (int) next_timeout > MAX_TIMOUT 
                    ? MAX_TIMOUT : next_timeout;
            } else {
                next_timeout = MAX_TIMOUT;
            }
            ret = epoll_wait(m_epfd, events, MAX_EVENtS, (int)next_timeout);
            if (ret < 0 &&errno == EINTR) {
                continue;
            } else {
                break;
            }
        } while(true);
        std::vector<std::function<void()> > cbs;
        listExpiredCb(cbs);
        if (!cbs.empty()) {
            schedul(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for (int i = 0; i < ret; ++i) {
            epoll_event& event = events[i];
            if (event.data.fd == m_tickleFds[0]) {
                uint8_t dummy[256];
                while(read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);
            if(event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_events = NONE;
            if (event.events & EPOLLIN) {
                real_events |= READ;
            }
            if (event.events & EPOLLOUT) {
                read_events |= WRITE;
            }
            if ((fd_ctx->events & read_events) == NONE) {
                continue;
            }
            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int ret_ = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if (ret_) {
                FANG_LOG_ERROR(g_logger) << "epoll_ctl error, "
                                        << strerror(errno);
                continue;
            }

            if (real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if (real_events & WRITE) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }

        }        
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swapOut();
    }
}


void IoManager::onTimerInsertedAtFront() {
    tickle();
}


IoManager::IoManager(size_t threads, bool use_caller, const std::string& name)
    :Scheduler(threads, use_caller, name) {
        m_epfd = epoll_create(5000);
        FANG_ASSERT(rt != -1);

        int ret = pipe(m_tickleFds);
        FANG_ASSERT(ret == 0);
        
        epoll_event event;
        memset(&event, 0, sizeof(event));
        event.events = EPOLLIN | EPOLLET;
        event.data.fd = m_tickleFds[0];

        ret = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
        FANG_ASSERT(ret == 0);

        ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
        FANG_ASSERT(ret == 0);
        contextResize(32);
        stack();
}

IoManager::~IoManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
        if (m_fdContexts[i] != nullptr) {
            delete m_fdContexts[i];
        }
    }
}

void IoManager::contextResize(size_t size) {
    m_fdContexts.resize(size);
    for (size_t i = 0; i < m_fdContexts.size(); i++) {
        if (m_fdContexts[i] == nullptr) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

int IoManager::addEvent(int fd, Event event, std::function<void()> cb) {
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if((int)m_fdContexts.size() > fd) {
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    } else {
        lock.unlock();
        RWMutexType::WrLock lock(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }
    
    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    
    
    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;
    int ret = epoll_ctl(m_epfd, op, fd, &epevent);

    if (ret) {
        FANG_LOG_ERROR(g_logger) <<  "epoll_ctl error," 
            << strerror(errno);
        return -1;
    }

    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event);

    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);

    event_ctx.Scheduler = fang::Scheduler::GetThis();
    if (cb) {
        event_ctx.swap(cb);
    } else {
        event_ctx.fiber = Fiber::GetThis();
    }
    return 0;
}

bool IoManager::delEvent(int fd, Event event) {
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    
    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);

    if (ret) {
        FANG_LOG_ERROR(g_logger) << "epoll_ctl error, " 
                            << strerror(errno);
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->events = new_events;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}


bool IoManager::cancelEvent(int fd, Event event) {
    RWMutexType::RdLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if ((fd_ctx->events & event) == 0) {
        return false;
    }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if (ret) {
        FANG_LOG_ERROR(g_logger) << "epoll_ctl error,"
                            << strerror(errno);
        return false;
    }
    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;

}

bool IoManager::cancelAll(int fd) {
    RWMutexType::RdLock lock(m_mutex);
    if((int)m_fdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if (!fd_ctx->events) {
        return false;
    }

    imt op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;
    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if (ret) {
        FANG_LOG_ERROR(g_logger) << "epoll_ctl error, "
                                << strerror(errno);
        return false;
    }

    if (fd_ctx->events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if (fd_ctx->events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --,m_pendingEventCount;
    }
    return true;
}




IoManager::FdContext::EventContext& IoManager::FdContext::getContext(IoManager::Event event) {
    switch(event) {
        case IoManager::READ:
            return read;
    case IoManager::WRITE:
            return write; 
    }
    throw std::invalib_argument("getContext invalid event");
}


void IoManager::FdContext::resetContext(EventContext& ctx) {
    ctx.Scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IoManager::FdContext::triggerEvent(IoManager::Event event) {
    FANG_ASSERT(events & event);
    events = (Event)(events & ~event);
    EventContext& ctx = getContext(event);
    if (ctx.cb) {
        ctx.scheduler->schedul(&ctx.cb);
    } else {
        ctx.scheduler->schedul(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}
}
