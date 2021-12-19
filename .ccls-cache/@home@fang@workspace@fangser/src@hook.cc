#include "../inc/hook.h"
#include <dlfcn.h>
#include <iostream>
#include <stdarg.h>

namespace fang{

static thread_local bool t_hook_enable = false;

#define HOOK_FUN()\
    DEF_XX(sleep)\
    DEF_XX(usleep)\
    DEF_XX(socket)\
    DEF_XX(connect)\
    DEF_XX(accept)\
    DEF_XX(read)\
    DEF_XX(readv)\
    DEF_XX(recv)\
    DEF_XX(recvfrom)\
    DEF_XX(recvmsg)\
    DEF_XX(write)\
    DEF_XX(writev)\
    DEF_XX(send)\
    DEF_XX(sendto)\
    DEF_XX(sendmsg)\
    DEF_XX(close)\
    DEF_XX(fcntl)\
    DEF_XX(getsockopt)\
    DEF_XX(setsockopt)\
    DEF_XX(ioctl)

void hook_init() {
    static bool is_inited = false;
    if (is_inited) {
        return;
    }
#define DEF_XX(name) name##_f = (name##_fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN();
#undef DEF_XX
}

bool is_hook_enable(){
    return t_hook_enable;
}

void set_hook_enable(bool flag) {
    t_hook_enable = flag;
}

struct _HookIniter {
    _HookIniter() {
        hook_init();
    }
};

static _HookIniter s_hook_initer;


}


struct timer_info {
    int cancelled =0;
};


template <typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun fun, const char * hook_fun_name,
        uint32_t event, int timeout_so, Args&&... args) {
    
    if (!fang::t_hook_enable) {//线程没有开启hook，就执行本来的函数
        return fun(fd, std::forward<Args>(args)...);
    }

    fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(fd);//从io列表中获取fd的数据

    if (!ctx) {
        return fun(fd, std::forward<Args>(args)...);
    }

    if (ctx->isClose()) {
        errno = EBADF;
        return -1;
    }

    if (!ctx->isSocket() || ctx->getUserNonblock()) {
        return fun(fd, std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);

    ssize_t n = fun(fd, std::forward<Args>(args)...);
    while(n == -1 && errno == EINTR) {
        n = fun(fd, std::forward<Args>(args)...);
    }
    if (n == -1 && errno == EAGAIN) {//返回错误设置为重试，说明io还没有数据可操作，故进行异步处理，把该io任务添加到任务列表，然后让出cpu去执行其他任务
        //do somethings;
        return to;
    }
    return 0;
}

extern "C" {
#define DEF_XX(name) name##_fun name##_f = nullptr;
    HOOK_FUN();
#undef DEF_XX
        
//sleep
unsigned int sleep(unsigned int seconds) {            
    if(!fang::t_hook_enable) {
        return sleep_f(seconds);
    }

    std::cout <<"sleep running!" <<std::endl;
    return 0;

}

int usleep(useconds_t usec) {
    if (!fang::t_hook_enable) {
        return usleep_f(usec);
    }

    std::cout << "usleep running!" <<std::endl;
    return 0;
}
        
//socket
int socket(int domain, int type, int protocol) {
    if (!fang::t_hook_enable) {
        return socket_f(domain, type, protocol);
    }
    int fd = socket_f(domain, type, protocol);//尝试socket连接
    if (fd == -1) {
        return fd;
    }
    fang::FdMgr::GetInstance()->get(fd, true);//把socketfd添加到io任务列表
    return fd;
}


int connect_with_timeout(int fd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout_ms) {
    if (!fang::t_hook_enable) {
        return connect_f(fd, addr, addrlen);
    }

    fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(fd);
    if (!ctx || ctx->isClose()) {
        errno = EBADF;
        return -1;
    }
    if (!ctx->isSocket()) {
        return connect_f(fd, addr, addrlen);
    }

    if (ctx->getUserNonblock()) {
        return connect_f(fd, addr, addrlen);
    }
    int n = connect_f(fd, addr, addrlen);
    if (n == 0) {
        return 0;
    } else if (n != -1 || errno != EINPROGRESS) {
        return n;
    }

    //dosthing
    return n;

}
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_f(sockfd, addr, addrlen, -1);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int fd = do_io(sockfd, accept_f, "accept", 0, SO_RCVTIMEO, addr, addrlen);
    if (fd > 0) {
        fang::FdMgr::GetInstance()->get(fd);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count) {
    return do_io(fd, read_f, "read", 0, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, readv_f, "readv", 0, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return do_io(sockfd, recv_f, "recv", 0, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return do_io(sockfd, recvfrom_f, "recvfrom", 0, SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}

ssize_t recvms(int sockfd, struct msghdr *msg, int flags) {
    return do_io(sockfd, recvmsg_f, "recvmsg", 0, SO_RCVTIMEO, msg, flags);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return do_io(fd, write_f, "write", 1, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return do_io(fd, writev_f, "writev", 1, SO_SNDTIMEO, iov, iovcnt);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return do_io(sockfd, send_f, "send", 1, SO_SNDTIMEO,  buf, len, flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    return do_io(sockfd, sendto_f, "sendto", 1, SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return do_io(sockfd, sendmsg_f, "sendmsg", 1, SO_SNDTIMEO, msg, flags);
}

int close(int fd) {
    if (!fang::t_hook_enable) {
        return close_f(fd);
    }
    fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(fd);
    if (ctx) {
        //do somethings

        fang::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}


int fcntl(int fd, int cmd, ...) {
    va_list va;
    va_start(va, cmd);
    switch(cmd) {
        case F_SETFL:
            {
                int arg =  va_arg(va, int);
                va_end(va);
                fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(fd);//查找io任务
                if (!ctx || ctx->isClose() || !ctx->isSocket()) {
                    return fcntl_f(fd, cmd, arg);
                }
                ctx->setUserNonblock(arg & O_NONBLOCK);//根据用户调用fcntl来设置用户是否设置fd是否阻塞
                if (ctx->getSysNonblock()) {
                    arg |= O_NONBLOCK;//如果系统参数设置为阻塞。则把io设置为阻塞
                } else {
                    arg &= ~O_NONBLOCK;//没有则设为非阻塞
                }
                return fcntl_f(fd, cmd, arg);//调用真正的函数，返回
            }
            break;

        case F_GETFL:
            {
                va_end(va);
                int arg = fcntl_f(fd, cmd);
                fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(fd);
                if (!ctx || ctx->isClose() ||!ctx->isSocket()) {
                    return arg;
                }
                if (ctx->getUserNonblock()) {
                    return arg | O_NONBLOCK;
                } else {
                    return arg & ~O_NONBLOCK;
                }
            }
            break;

        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
        case F_SETPIPE_SZ:
            {
                int arg = va_arg(va, int);
                va_end(va);
                return fcntl_f(fd, cmd, arg); 
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
        case F_GETPIPE_SZ:
            {
                va_end(va);
                return fcntl_f(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return fcntl_f(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

int ioctl(int fd, unsigned long request, ...) {
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void *);
    va_end(va);

    if (FIONBIO == request) {
        bool user_nonblock = !!*(int *)arg;
        fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(fd);
        if (!ctx || ctx->isClose() ||!ctx->isSocket()) {
            return ioctl_f(fd, request, arg);
        }
        ctx->setUserNonblock(user_nonblock);
    }
    return ioctl_f(fd, request, arg);
}



int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (!fang::t_hook_enable) {
        setsockopt_f(sockfd, level, optname, optval, optlen);
    }
    if (level == SOL_SOCKET) {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            fang::FdCtx::ptr ctx = fang::FdMgr::GetInstance()->get(sockfd);
            if (ctx) {
                const struct timeval *v = (const timeval*)optval;
                ctx->setTimeout(optname, v->tv_usec / 1000 +v->tv_sec * 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}
}
