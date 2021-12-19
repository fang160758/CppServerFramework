#ifndef __FANG_MUTEX_H__
#define __FANG_MUTEX_H__
#include "singleton.h"
#include <semaphore.h>
#include <pthread.h>
#include <atomic>



namespace fang {


    //信号量
    class Semaphore : Noncopyable {
        public:
            Semaphore(unsigned int count = 0);
            ~Semaphore();
            
            //获取信号量    
            void wait();

            //释放信号量
            void post();

        private:
           sem_t m_semaphore;//信号量变量            
    };



    //局部互斥锁模版
    template <typename T>
    struct ScopedLockImpl {
        public:
            ScopedLockImpl(T& mutex)
                :m_mutex(mutex) {
                    m_mutex.lock();//自动锁上
                    m_locked = true;
                }

            ~ScopedLockImpl() {
                unlock();//自动解锁
            }

            //加锁
            void lock() {
                if (!m_locked) {
                    m_mutex.lock();
                    m_locked = true;
                }
            }

            //解锁
            void unlock() {
                if (m_locked) {
                    m_mutex.unlock();
                    m_locked = false;
                }
            }

        private:
            T& m_mutex;//锁
            bool m_locked;//记录是否上锁
    };


    //局部互斥读锁模版
    template <typename T>
    struct ReadScopedLockImpl {
        public:
            
            ReadScopedLockImpl(T& mutex)
                : m_mutex(mutex) {
                    m_mutex.rdlock();//加上读锁
                    m_locked = true;
                }
            ~ReadScopedLockImpl() {
                unlock();
            }
            
            void lock() {
                if (!m_locked) {
                    m_mutex.rdlock();
                    m_locked = true;
                }
            }

            void unlock() {
                if (m_locked) {
                    m_mutex.unlock();
                    m_locked = false;
                }
            }

        private:
            T& m_mutex;
            bool m_locked;

    };

    //局部写锁模版
    template <typename T>
    struct WriteScopedLockImpl {
        public:
            
            WriteScopedLockImpl(T& mutex)
                : m_mutex(mutex) {
                    m_mutex.wrlock();//加上写锁
                    m_locked = true;
                }
            ~WriteScopedLockImpl() {
                unlock();
            }
            
            void lock() {
                if (!m_locked) {
                    m_mutex.wrlock();
                    m_locked = true;
                }
            }

            void unlock() {
                if (m_locked) {
                    m_mutex.unlock();
                    m_locked = false;
                }
            }

        private:
            T& m_mutex;
            bool m_locked;

    };


    //互斥量
    class Mutex : Noncopyable {
        public:
            typedef ScopedLockImpl<Mutex> Lock;
        public:
            Mutex() {
                //初始化
                pthread_mutex_init(&m_mutex, nullptr);
            }        
            
            ~Mutex() {
                //销毁
                pthread_mutex_destroy(&m_mutex);
            }

            void lock() {
                //上锁
                pthread_mutex_lock(&m_mutex);
            }

            void unlock() {
                //解锁
                pthread_mutex_unlock(&m_mutex);
            }
    
        
        private:
            pthread_mutex_t m_mutex;//互斥量
    };

    //读写锁
    class RWMutex : Noncopyable {
        public:
            typedef ReadScopedLockImpl<RWMutex> RdLock;
            typedef WriteScopedLockImpl<RWMutex> WrLock;
        public:
            RWMutex() {
                //初始化
                pthread_rwlock_init(&m_lock, nullptr);
            }

            ~RWMutex() {
                //销毁
                pthread_rwlock_destroy(&m_lock);
            }

            void rdlock() {
                //上读锁
                pthread_rwlock_rdlock(&m_lock);
            }

            void wrlock() {
                //上写锁
                pthread_rwlock_wrlock(&m_lock);
            }

            void unlock() {
                //解锁
                pthread_rwlock_unlock(&m_lock);
            }

        private:
            pthread_rwlock_t m_lock;//读写锁
    };

    //自旋锁
    class Spinlock : Noncopyable {
        public:
            typedef ScopedLockImpl<Spinlock> Lock;

        public:
            Spinlock() {
                pthread_spin_init(&m_mutex, 0);
            }
            
            ~Spinlock() {
                pthread_spin_destroy(&m_mutex);
            }    

            void lock() {
                pthread_spin_lock(&m_mutex);
            }

            void unlock() {
                pthread_spin_unlock(&m_mutex);
            }

        private:
            pthread_spinlock_t m_mutex;
    };


    //原子锁
    class CASLock : Noncopyable {
        public:
            typedef ScopedLockImpl<CASLock> Lock;
        
        public:
            CASLock() {
                m_mutex.clear();
            }

            ~CASLock() {

            }

            void lock() {
                while(std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
            }

            void unlock() {
                std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
            }

        private:
            volatile std::atomic_flag m_mutex;//原子状态
    };

}



#endif
