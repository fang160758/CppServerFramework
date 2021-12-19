#include "../inc/mutex.h"
#include <stdexcept>


namespace fang {
    Semaphore::Semaphore(unsigned int count) {
        //初始化信号量值为count
        if (sem_init(&m_semaphore, 0, count) == -1) {
            throw std::logic_error("sem_init error!");
        }
    }

    Semaphore::~Semaphore() {
        //销毁信号量
        sem_destroy(&m_semaphore);
    }

    void Semaphore::wait() {
        //获取信号量
        if (sem_wait(&m_semaphore) == -1) {
            throw std::logic_error("sem_wait error!");
        }
    }

    void Semaphore::post() {
        //释放信号量
        if (sem_post(&m_semaphore) == -1) {
            throw std::logic_error("sem_post error!");
        }
    }



}
