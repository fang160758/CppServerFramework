#include "../inc/socket.h"
#include "../inc/fd_manager.h"
//#include "../inc/mydef.h"
#include <netinet/tcp.h>

#include "../inc/hook.h"

namespace fang {

    Socket::ptr Socket::CreateTCPScoket() {
        
        Socket::ptr sock(new Socket(IPv4, TCP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateUDPScoket() {

        Socket::ptr sock(new Socket(IPv4, UDP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateTCPScoket6() {

        Socket::ptr sock(new Socket(IPv6, TCP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateUDPScoket6() {
        
        Socket::ptr sock(new Socket(IPv6, UDP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateTCP(Address::ptr addr) {
        Socket::ptr sock(new Socket(addr->getFamily(), TCP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateUDP(Address::ptr addr) {

        Socket::ptr sock(new Socket(addr->getFamily(), TCP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateUnixTCPScoket() {

        Socket::ptr sock(new Socket(UNIX, TCP, 0));
        return sock;
    }
    Socket::ptr Socket::CreateUnixUDPScoket() {

        Socket::ptr sock(new Socket(UNIX, UDP, 0));
        return sock;
    }

    Socket::Socket(int family, int type, int protocol)
    : m_sock(-1)
    , m_family(family)
    , m_type(type)
    , m_protocol(protocol)
    , m_isConnected(false) {}
    
    Socket::~Socket() {
        close();
    }
    uint64_t Socket::getSendTimeout() {
        //todo
        FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_sock);
        if (ctx) {
            return ctx->getTimeout(SO_SNDTIMEO);
        }
        return -1;
    }

    void Socket::setSendTimeout(uint64_t v) {
        struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
        setOption(SOL_SOCKET, SO_SNDTIMEO, (const void*)&tv, (socklen_t)sizeof(tv));
    }

    uint64_t Socket::getRecvTimeout() {
        FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_sock);
        if (ctx) {
            return ctx->getTimeout(SO_RCVTIMEO);
        }
        return -1;
    }

    void Socket::setRecvTimeout(uint64_t v) {
        struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
        setOption(SOL_SOCKET, SO_RCVTIMEO, (const void*)&tv, (socklen_t)sizeof(tv));
    }

    bool Socket::getOption(int level, int optname, void* result, socklen_t* len) {
        int ret = getsockopt(m_sock, level, optname, result, (socklen_t*)len);
        if (ret == -1) {
            perror(strerror(errno));
            return false;
        }
        return true;
    }

    bool Socket::setOption(int level, int optname, const void* result, socklen_t len) {
        int ret = setsockopt(m_sock, level, optname, result, (socklen_t)len);
        if (ret == -1) {
            perror(strerror(errno));
            return false;
        }
        return true;
    }


            
    Socket::ptr Socket::accept() {
        Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
        int ret = ::accept(m_sock, nullptr, nullptr);
        if (ret == -1) {
            perror(strerror(errno));
            return nullptr;
        }
        if (sock->init(ret)) {
            return sock;
        }
        return nullptr;
    }

    bool Socket::init(int sock) {
        FdCtx::ptr ctx = FdMgr::GetInstance()->get(sock);
        if (ctx && ctx->isSocket() && !ctx->isClose()) {
            m_sock = sock;
            m_isConnected = true;
            initSock();
            //do something        
            getLocalAddress();
            getRemoteAddress();
            return true;
        }
        return false;
    }

    bool Socket::bind(const Address::ptr addr) {
        if (!isValid()) {
            newSock();
            if (!isValid()){
                return false;
            }
        }
        if (addr->getFamily() != m_family) {
            perror("addr->family != m_family");
            return false;
        }

        if (::bind(m_sock, addr->getAddr(), addr->getAddrLen())) {
            perror("bind error");
        }
        getLocalAddress();
        return true;
    }

    bool Socket::connect(const Address::ptr addr, uint64_t timeout_ms) {
        if (!isValid()) {
            newSock();
            if (!isValid()) {
                return false;
            }
        }
        if (addr->getFamily() != m_family) {
            perror("addr->family != m_family");
            return false;
        }

        if (timeout_ms == (uint64_t)-1) {
            if (::connect(m_sock, addr->getAddr(), addr->getAddrLen())) {
                perror(strerror(errno));
                close();
                return false;
            }
        } else {
            if (::connect_with_timeout(m_sock, addr->getAddr(), addr->getAddrLen(), timeout_ms)) {
                perror(strerror(errno));
                close();
                return false;
            }
        }

        m_isConnected = true;
        getLocalAddress();
        getRemoteAddress();
        return true;
    }


    bool Socket::listen(int backlog) {
        if(!isValid()) {
            perror("listen error");
            return false;
        }
        if (::listen(m_sock, backlog)) {
            perror(strerror(errno));
            return false;
        }
        return true;
    }

    bool Socket::close() {
        if (!m_isConnected && m_sock == -1) {
            return true;
        }
        m_isConnected = false;
        if (m_sock != -1) {
            ::close(m_sock);
            m_sock = -1;
        }
        return false;
    }

    int Socket::send(const void* buffer, size_t length, int flags) {
        if (m_isConnected) {
            return ::send(m_sock, buffer, length, flags);
        }
        return -1;
    }

    int Socket::send(const struct iovec* buffers, size_t length, int flags) {
        if (m_isConnected) {
            struct msghdr msgh;
            memset(&msgh, 0, sizeof(msgh));
            msgh.msg_iov = const_cast<iovec*>(buffers);
            msgh.msg_iovlen = length;
            return ::sendmsg(m_sock, &msgh, flags);
        }
        return -1;
    }

    int Socket::sendTo(const void* buffer, size_t length, Address::ptr addr, int flags) {
        if (m_isConnected) {
            return ::sendto(m_sock, buffer, length, flags, addr->getAddr(), addr->getAddrLen());
        }
        return -1;
    }

    int Socket::sendTo(const struct iovec* buffers, size_t length, Address::ptr addr, int flags) {
        if (m_isConnected) {
            struct msghdr msg;
            memset(&msg, 0, sizeof(msg));
            msg.msg_iov = (iovec*)buffers;
            msg.msg_iovlen = length;
            msg.msg_name = addr->getAddr();
            msg.msg_namelen = addr->getAddrLen();
            return ::sendmsg(m_sock, &msg, flags);
        }
        return -1;
    }

    int Socket::recv(void* buffer, size_t length, int flags) {
        if (m_isConnected) {
            return ::recv(m_sock, buffer, length, flags);
        }
        return -1;
    }

    int Socket::recv(struct iovec* buffers, size_t length, int flags) {
        if (m_isConnected) {
            struct msghdr msgh;
            memset(&msgh, 0, sizeof(msgh));
            msgh.msg_iov = const_cast<iovec*>(buffers);
            msgh.msg_iovlen = length;
            return ::recvmsg(m_sock, &msgh, flags);
        }
        return -1;

    }
    int Socket::recvFrom(void* buffer, size_t length, Address::ptr addr, int flags) {
        if(m_isConnected) {
            int len = addr->getAddrLen();
            return ::recvfrom(m_sock, buffer, length, flags, const_cast<sockaddr*>(addr->getAddr()), (socklen_t*)&len);
        }
        return -1;
    }

    int Socket::recvFrom(struct iovec* buffers, size_t length, Address::ptr addr, int flags) {
          if (m_isConnected) {
            struct msghdr msg;
            memset(&msg, 0, sizeof(msg));
            msg.msg_iov = (iovec*)buffers;
            msg.msg_iovlen = length;
            msg.msg_name = addr->getAddr();
            msg.msg_namelen = addr->getAddrLen();
            return ::recvmsg(m_sock, &msg, flags);
        }
        return -1;

    }
    Address::ptr Socket::getLocalAddress() {
 if (m_localAddress) {
            return m_localAddress;
        }

        Address::ptr result;
        switch(m_family){
            case AF_INET:
            result.reset(new IPv4Address());
            break;
            case AF_INET6:
            result.reset(new IPv6Address());
            break;
            case AF_UNIX:
            result.reset(new UnixAddress());
            default:
            result.reset(new UnknowAddress(m_family));
            break;
        }
        socklen_t addrlen = result->getAddrLen();
        if(getsockname(m_sock, result->getAddr(), &addrlen)) {
            perror(strerror(errno));
            return Address::ptr(new UnknowAddress(m_family));
        }
        if (m_family == AF_UNIX) {
            UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
            addr->setAddrLen(addrlen);
        }

        m_localAddress = result;
        return m_localAddress;

    }
    Address::ptr Socket::getRemoteAddress() {
        if (m_remoteAddress) {
            return m_remoteAddress;
        }

        Address::ptr result;
        switch(m_family){
            case AF_INET:
            result.reset(new IPv4Address());
            break;
            case AF_INET6:
            result.reset(new IPv6Address());
            break;
            case AF_UNIX:
            result.reset(new UnixAddress());
            default:
            result.reset(new UnknowAddress(m_family));
            break;
        }
        socklen_t addrlen = result->getAddrLen();
        if(getpeername(m_sock, result->getAddr(), &addrlen)) {
            perror(strerror(errno));
            return Address::ptr(new UnknowAddress(m_family));
        }
        if (m_family == AF_UNIX) {
            UnixAddress::ptr addr = std::dynamic_pointer_cast<UnixAddress>(result);
            addr->setAddrLen(addrlen);
        }

        m_remoteAddress = result;
        return m_remoteAddress;
}
        
int Socket::getError() {
    int error = 0;
    int len = sizeof(error);
    int ret = getOption(SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len);

    if (ret == -1) {
        return -1;
    }
    return error;
}

bool Socket::cancelRead(){ return true; }
bool Socket::cancelWirte(){ return true; }
bool Socket::cancelAll(){ return true; }

bool Socket::isValid() const {
    return m_sock != -1;
}

std::ostream& Socket::dump(std::ostream& os) const {
    os << "[Socket sock=" << m_sock
       << " is_connected=" << m_isConnected
       << " family=" << m_family
       << " typ=" << m_type
       << " protocol=" << m_protocol;
    if (m_localAddress) {
        os << " localAddress=" << m_localAddress->toString();
    }
    if (m_remoteAddress) {
        os << " remoteAddress=" << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

void Socket::initSock() {
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, &val);
    if (m_type == SOCK_STREAM) {
        setOption(IPPROTO_TCP, TCP_NODELAY, &val);
    }
}

void Socket::newSock() {
    int sock = socket(m_family, m_type, m_protocol);
    if (sock != -1){
        initSock();
    } else {
        fprintf(stderr, "socket error");
    }
}

}
