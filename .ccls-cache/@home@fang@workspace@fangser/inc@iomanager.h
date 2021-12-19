#pragma once 
#include "scheduler.h"
#include "timer.h"

namespace fang {

class IoManager : public Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IoManager> ptr;
    typedef RWMutex RWMutexType;
    
    enum Event{
        NONE = 0x0,
        READ = 0x1,
        WRITE = 0x2,
    };

public:
    IoManager(size_t threads = -1, bool use_caller = false, const std::string& name = "");
    ~IoManager();


    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    bool delEvent(int fd, Event event);

    bool cancelEvent(int fd, Event event);
    bool cancelAll(int fd);

    static IoManager* GetThis();

protected:
    void tickle()   override;
    bool stopping() override;
    void idle()     override;
    void onTimerInsertedAtFront() override;

    void contextResize(size_t size);
    bool stopping(uint64_t& timeout);


private:
    struct FdContext {
        typedef Mutex MutexType;

        struct EventContext {
            Scheduler* scheduler = nullptr;
            Fiber::ptr fiber;
            std::function<void()> cb;
        };

        EventContext& getContext(Event event);

        void resetContext(EventContext& ctx);

        void triggerEvent(Event event);

        EventContext read;
        EventContext write;
        int fd = 0;
        Event events = NONE;
        Mutex mutex;
    };

private:
    int m_epfd;
    int m_tickleFds[2];
    std::atomic<size_t> m_pendingEventCount = {0};
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts; 
};
}
