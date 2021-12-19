#ifndef __FANG_THREAD_H__
#define __FANG_THREAD_H__
#include "singleton.h"
#include "mutex.h"
#include <pthread.h>
#include <memory>
#include <functional>
#include <string.h>

namespace fang {
 
class Thread : Noncopyable {
    public:
        typedef std::shared_ptr<Thread> ptr;    

        Thread(std::function<void()> cb, const std::string& name = "UNKNOW");
        ~Thread();

        pid_t getId() const { return m_id; }
        const std::string& getName() const { return m_name;}
        void join();    //等待线程执行完毕
        static Thread* GetThis();   //获取当前的线程指针
        static const std::string& GetName();    //获取当前执行线程的名称
        static void SetName(const std::string& name);   //设置当前执行线程的名称

    private:
        static void *run(void* arg);    //线程执行函数
    private:
        pid_t m_id = -1;                //线程ID
        pthread_t m_thread = 0;         //线程结构
        std::function<void()> m_cb;     //线程执行函数
        std::string m_name;             //线程名称
        Semaphore m_semaphore;          //信号量
};
}




#endif
