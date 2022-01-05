#pragma once 
#include "scheduler.h"
#include "timer.h"

namespace fang {

/**
* @Synopsis  基于epoll的io协程调度器
*/
class IoManager : public Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IoManager> ptr;
    typedef RWMutex RWMutexType;
    
    /**
     * @Synopsis  IO时间
     */
    enum Event{
        NONE = 0x0,     //无事件
        READ = 0x1,     //读事件
        WRITE = 0x2,    //写事件
    };

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

public:

    /**
     * @Synopsis  构造器
     *
     * @Param[in] threads 线程数量
     * @Param[in] use_caller 是否将系统线程作为调度
     * @Param[in] name 名称
     */
    IoManager(size_t threads = -1, bool use_caller = false, const std::string& name = "");
    ~IoManager();


    /**
     * @Synopsis  添加事件
     *
     * @Param[in] fd socket文件描述符
     * @Param[in] event 事件类型
     * @Param[in] cb 事件回调函数
     *
     * @Returns 成功返回0， 失败返回-1
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    
    /**
     * @Synopsis  删除事件，不会触发事件回调
     *
     * @Param[in] fd socket文件描述符
     * @Param[in] event 事件类型
     */
    bool delEvent(int fd, Event event);

    /**
     * @Synopsis  取消事件，如果事件存在就触发事件
     *
     * @Param[in] fd socket文件描述符
     * @Param[in] event 事件类型
     */
    bool cancelEvent(int fd, Event event);
    
    /**
     * @Synopsis  取消所有事件，事件存在就会去触发
     *
     * @Param[in] fd socket文件描述符
     */
    bool cancelAll(int fd);

    /**
     * @Synopsis  获得当前指针
     */
    static IoManager* GetThis();

protected:
    void tickle()   override;
    bool stopping() override;
    void idle()     override;
    void onTimerInsertedAtFront() override;

    void contextResize(size_t size);
    bool stopping(uint64_t& timeout);

private:
    int m_epfd; // epoll 句柄
    int m_tickleFds[2]; // pipe 文件描述符
    std::atomic<size_t> m_pendingEventCount = {0}; //就绪事件数量
    RWMutexType m_mutex;    //读写锁
    std::vector<FdContext*> m_fdContexts; // socket事件上下文容器 
};
}
