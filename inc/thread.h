/**
 * @file thread.h
 * @Synopsis  线程封装类
 * @author Fang
 * @version 1.0
 * @date 2022-01-01
 */

#ifndef __FANG_THREAD_H__
#define __FANG_THREAD_H__
#include "singleton.h"
#include "mutex.h"
#include <pthread.h>
#include <memory>
#include <functional>
#include <string.h>

namespace fang {
 
/**
* @Synopsis  线程类
*/
class Thread : Noncopyable {
public:
    typedef std::shared_ptr<Thread> ptr;    

    /**
     * @Synopsis  构造函数
     *
     * @Param[in] cb 线程调用函数
     * @Param[in] name 线程名称
     */
    Thread(std::function<void()> cb, const std::string& name = "UNKNOW");
    ~Thread();

    pid_t getId() const { return m_id; }
    const std::string& getName() const { return m_name;}

    /**
     * @Synopsis  等待线程执行完
     */
    void join(); 
     
    /**
     * @Synopsis  获取当前执行的线程指针
     *
     * @Returns   
     */
    static Thread* GetThis();   

    /**
     * @Synopsis  获取当前执行线程的名称
     */
    static const std::string& GetName(); 

    /**
     * @Synopsis  设置当前执行线程的名称
     */
    static void SetName(const std::string& name); 

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
