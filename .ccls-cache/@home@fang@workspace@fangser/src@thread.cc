#include "../inc/thread.h"
#include "../inc/log.h"
#include <errno.h>

namespace fang {

static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "NUKNOW";

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");


Thread* Thread::GetThis() { return t_thread; }

const std::string& Thread::GetName() { return t_thread_name; }

void Thread::SetName(const std::string& name) {
    if (name.empty()) {
        return ;
    }
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> cb, const std::string& name)
    : m_cb(cb)
    , m_name(name) {
    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this); //初始化线程结构
    if (ret != 0) { //返回非零，表示出错，打印日志，抛出异常
        FANG_LOG_ERROR(g_logger) << "pthread_create error,"
            << " thread name=" << m_name
            << " error=" << strerror(errno);
        throw std::logic_error("pthread_create error");
    }
    m_semaphore.wait(); //获取信号量
}


Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);   //如果线程结构没有被销毁就在析构函数进行回收
    }
}

void Thread::join() {
    if (m_thread) {
        int ret = pthread_join(m_thread, nullptr);  //等待线程执行完成
        if (ret != 0) { //返回非零，表示出错，打印日志，抛出异常
            FANG_LOG_ERROR(g_logger) << "pthread_join error,"
                << " thread name=" << m_name
                << " error=" << strerror(errno);
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run(void* arg) {
    Thread* thread = (Thread*)arg; //传进来一个Thread对象
    t_thread = thread;  //更新当前的线程变量
    t_thread_name = thread->getName();  //更新当前的线程名称
    thread->m_id = fang::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());    // 设置当前执行线程的名称

    std::function<void()> cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.post();     //释放线程的信号量
    cb();
    return 0;

}


}
