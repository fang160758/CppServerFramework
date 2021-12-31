#include "../inc/fd_manager.h"
#include "../inc/hook.h"
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

namespace fang {

FdCtx::FdCtx(int fd)
    : m_isInit(false)
    , m_isSocket(false)
    , m_sysNonblock(false)
    , m_userNonblock(false)
    , m_isClose(false)
    , m_fd(fd)
    , m_recvTimeout(-1)
    , m_sendTimeout(-1) {
        init();
}

bool FdCtx::init() {
    if (m_isInit) {
        return true;
    }
    m_recvTimeout = -1;
    m_sendTimeout = -1;

    struct stat fd_stat;
    if (fstat(m_fd, &fd_stat) == -1) {
        m_isInit = false;
        m_isSocket = false;
        perror(strerror(errno));            
    } else {
        m_isInit = true;
        m_isSocket = S_ISSOCK(fd_stat.st_mode);
    }

    if (m_isSocket) {
        int flags = fcntl_f(m_fd, F_GETFL, 0);
        if (!(flags & O_NONBLOCK)) { // 如果是socket文件描述符就设置为阻塞
            fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
        }
        m_sysNonblock = true;
    } else {
         m_sysNonblock = false;
    }
    m_userNonblock = false;
    m_isClose = false;
    return m_isInit;
}

void FdCtx::setTimeout(int type, uint64_t v) {
    if (type == SO_RCVTIMEO) {
        m_recvTimeout = v;
    } else {
        m_sendTimeout = v;
    }
}

uint64_t FdCtx::getTimeout(int type) {
    if (type == SO_RCVTIMEO) {
        return m_recvTimeout;
    } else {
        return m_sendTimeout;
    }
}

FdManager::FdManager() {
    m_datas.resize(64);
}

FdCtx::ptr FdManager::get(int fd, bool auto_create) {
    //读锁开启
    RWMutex::RdLock lock(m_mutex);
    if ((int)m_datas.size() <= fd) {
        if (auto_create == false) {
            return nullptr;
        }
    } else {
        if (m_datas[fd] || !auto_create) {
            return m_datas[fd];
        }
    }

    //读锁被关闭
    lock.unlock();
    //写锁被开启
    RWMutex::WrLock _lock(m_mutex);
    FdCtx::ptr fdctx(new FdCtx(fd));
    m_datas[fd] = fdctx;
    return  fdctx;
}

void FdManager::del(int fd) {
    //写锁开启
    RWMutex::WrLock lock(m_mutex);
    if ((int)m_datas.size() <= fd) {
        return ;
    }
    m_datas[fd].reset();
}
}
