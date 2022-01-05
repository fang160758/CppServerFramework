#include "../inc/socket.h"
#include "../inc/fd_manager.h"
#include "../inc/mydef.h"
#include "../inc/helpc.h"
#include <ctime>
#include <memory>
#include <netinet/tcp.h>
#include <ostream>
#include <string>
#include <sys/socket.h>
#include "../inc/log.h"
#include "../inc/hook.h"

namespace fang {
    static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");

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
        FdCtx::ptr ctx = FdMgr::GetInstance()->get(m_sock);
        if (ctx) {
            return ctx->getTimeout(SO_SNDTIMEO);
        }
        return -1;
    }

    void Socket::setSendTimeout(uint64_t v) {
        struct timeval tv{int(v / 1000), int(v % 1000 * 1000)};
        setOption(SOL_SOCKET, SO_SNDTIMEO, tv);
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
        setOption(SOL_SOCKET, SO_RCVTIMEO, tv);
    }

    bool Socket::getOption(int level, int optname, void* result, socklen_t* len) {
        int ret = getsockopt(m_sock, level, optname, result, (socklen_t*)len);
        if (ret == -1) {
            FANG_LOG_ERROR(g_logger) << "Socket::getOption fail sock=" << m_sock
                << " level=" << level << " option=" << optname
                << " errno=" << errno << " errstr=" << strerror(errno);
            return false;
        }
        return true;
    }

    bool Socket::setOption(int level, int optname, const void* result, socklen_t len) {
        int ret = setsockopt(m_sock, level, optname, result, (socklen_t)len);
        if (ret == -1) {
            FANG_LOG_ERROR(g_logger) << "Socket::setOption fail sock=" << m_sock
                << " level=" << level << " option=" << optname
                << " errno=" << errno << " errstr=" << strerror(errno);
            return false;
        }
        return true;
    }
            
    Socket::ptr Socket::accept() {
        Socket::ptr sock(new Socket(m_family, m_type, m_protocol));
        int ret = ::accept(m_sock, nullptr, nullptr);
        if (ret == -1) {
            FANG_LOG_ERROR(g_logger) << "Socket::accept fail sock=" << m_sock
                << " errno=" << errno << " errstr=" << strerror(errno);
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
            FANG_LOG_ERROR(g_logger) << "Socket::bind fail socket.family=" << m_family
                << " - addr.family=" << addr->getFamily()
                << " not equal, addr=" << addr->toString();
            return false;
        }

        UnixAddress::ptr uaddr = std::dynamic_pointer_cast<UnixAddress>(addr);
        if (addr) {
            Socket::ptr sock = Socket::CreateUnixTCPScoket();
            if (sock->connect(uaddr)) {
                return false;
            } else {
                fang::Helpc::Unlink(uaddr->getPath(), true);
            }
        }

        if (::bind(m_sock, addr->getAddr(), addr->getAddrLen())) {
            FANG_LOG_ERROR(g_logger) << "Socket::bind fail errno=" << errno
                << " errstr=" << strerror(errno);
            return false;
        }
        getLocalAddress();
        return true;
    }

    bool Socket::connect(const Address::ptr addr, uint64_t timeout_ms) {
        m_remoteAddress = addr;
        if (!isValid()) {
            newSock();
            if (!isValid()) {
                return false;
            }
        }
        if (addr->getFamily() != m_family) {
            FANG_LOG_ERROR(g_logger) << "Socket::connect fail socket.family=" << m_family
                << " and addr.family=" << addr->getFamily()
                << " not equal, addr=" << addr->toString();
            return false;
        }

        if (timeout_ms == (uint64_t)-1) {
            if (::connect(m_sock, addr->getAddr(), addr->getAddrLen())) {
                FANG_LOG_ERROR(g_logger) << "sock=" << m_sock <<" connect(" << addr->toString()
                    << ") error errno=" << errno << " errstr=" << strerror(errno);
                close();
                return false;
            }
        } else {
            if (::connect_with_timeout(m_sock, addr->getAddr(), addr->getAddrLen(), timeout_ms)) {
                FANG_LOG_ERROR(g_logger) << "sock=" << m_sock <<" connect(" << addr->toString()
                    << ") timeout= " << timeout_ms << "error errno=" << errno
                    << " errstr=" << strerror(errno);
                close();
                return false;
            }
        }

        m_isConnected = true;
        getLocalAddress();
        getRemoteAddress();
        return true;
    }
    
    bool Socket::reconnect(uint64_t timeout_ms) {
        if (!m_remoteAddress) {
            FANG_LOG_ERROR(g_logger) << "reconnect m_remoteAddress is nullptrl";
            return false;
        }
        m_localAddress.reset();
        return connect(m_remoteAddress, timeout_ms);
    }

    bool Socket::listen(int backlog) {
        if(!isValid()) {
            FANG_LOG_ERROR(g_logger) << "listen error sock=-1";
            return false;
        }
        if (::listen(m_sock, backlog)) {
            FANG_LOG_ERROR(g_logger) << "Socket::listen fail sock=" << m_sock
                << " errno=" << errno << " errstr=" << strerror(errno);
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
            FANG_LOG_ERROR(g_logger) << "getsockname error sock=" << m_sock
                << " errno=" << errno << " errstr=" << strerror(errno);
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
            FANG_LOG_ERROR(g_logger) << "getpeername error sock=" << m_sock
                << " errno=" << errno << " errstr=" << strerror(errno);
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

bool Socket::cancelRead() { return true; }
bool Socket::cancelWirte(){ return true; }
bool Socket::cancelAll()  { return true; }

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

std::string Socket::toString() const {
    std::stringstream ss;
    dump(ss);
    return ss.str();
}

void Socket::initSock() {
    int val = 1;
    setOption(SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (m_type == SOCK_STREAM) {
        setOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));
    }
}

void Socket::newSock() {
    int sock = socket(m_family, m_type, m_protocol);
    if (sock != -1){
        initSock();
    } else {
        FANG_LOG_ERROR(g_logger) << "socket(" << m_family
            << ", " << m_type << ", " << m_protocol << ") errno="
           << errno << " errstr" <<  strerror(errno);
    }
}

namespace {
    struct __SSLInit {
        __SSLInit() {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();
        }
    };
static __SSLInit s_init;

}


SSLSocket::SSLSocket(int family, int type, int protocol)
    : Socket(family, type, protocol) {}


    Socket::ptr SSLSocket::accept() {
        SSLSocket::ptr sock(new SSLSocket(m_family, m_type, m_protocol));
        int newsock = ::accept(m_sock, nullptr, nullptr);
        if (newsock == -1) {
            FANG_LOG_ERROR(g_logger) << "accept(" << m_sock << ") errno="
                << errno << " errstr=" << strerror(errno);
            return nullptr;
        }
        sock->m_ctx = m_ctx;
        if (sock->init(newsock)) {
            return sock;
        }
        return nullptr;
    }

bool SSLSocket::bind(const Address::ptr addr) {
    return Socket::bind(addr);
}

bool SSLSocket::connect(const Address::ptr addr, uint64_t timeout_ms) {
    bool v = Socket::connect(addr, timeout_ms);
    if (v) {
        m_ctx.reset(SSL_CTX_new(SSLv23_client_method()), SSL_CTX_free);
        m_ssl.reset(SSL_new(m_ctx.get()), SSL_free);
        SSL_set_fd(m_ssl.get(), m_sock);
        v = (SSL_connect(m_ssl.get()) == 1);
    }
    return v;
}

bool SSLSocket::listen(int backlog) {
    return Socket::listen(backlog);
}

bool SSLSocket::close() {
    return Socket::close();
}

int SSLSocket::send(const void* buffer, size_t length, int flags) {
    if (m_ssl) {
        return SSL_write(m_ssl.get(), buffer, length);
    }
    return -1;
}
int SSLSocket::send(const struct iovec* buffers, size_t length, int flags) {
    if (!m_ssl) {
        return -1;
    }
    int total = 0;
    for (size_t i = 0; i< length; ++i) {
        int tmp = SSL_write(m_ssl.get(), buffers[i].iov_base, buffers[i].iov_len);
        if (tmp <= 0) {
            return tmp;
        }
        total += tmp;
        if (tmp != (int)buffers[i].iov_len) {
            break;
        }
    }
    return total;
}
int SSLSocket::sendTo(const void* buffer, size_t length, Address::ptr, int flag) {
    FANG_ASSERT(false);
    return -1;
}
int SSLSocket::sendTo(const struct iovec* buffers, size_t length, Address::ptr, int flags) {
    FANG_ASSERT(false);
    return -1;
}

int SSLSocket::recv(void* buffer, size_t length, int flags) {
    if (m_ssl) {
        return SSL_read(m_ssl.get(), buffer, length);
    }
    return -1;
}
int SSLSocket::recv(struct iovec* buffers, size_t length, int flags) {
    if (!m_ssl) {
        return -1;
    }
    int total = 0;
    for (size_t i = 0; i< length; ++i) {
        int tmp = SSL_read(m_ssl.get(), buffers[i].iov_base, buffers[i].iov_len);
        if (tmp <= 0) {
            return tmp;
        }
        total += tmp;
        if (tmp != (int)buffers[i].iov_len) {
            break;
        }
    }
    return total;
}
int SSLSocket::recvFrom(void* buffer, size_t length, Address::ptr, int flags) {
    FANG_ASSERT(false);
    return -1;
}
int SSLSocket::recvFrom(struct iovec* buffers, size_t length, Address::ptr, int flags) {
    FANG_ASSERT(false);
    return -1;
}

bool SSLSocket::init(int sock) {
    bool v = Socket::init(sock);
    if (v) {
        m_ssl.reset(SSL_new(m_ctx.get()), SSL_free);
        SSL_set_fd(m_ssl.get(), m_sock);
        v = (SSL_accept(m_ssl.get()) == -1);   
    }
    return v;
}

bool SSLSocket::loadCertificates(const std::string& cerf_file, const std::string& key_file) {
    m_ctx.reset(SSL_CTX_new(SSLv23_client_method()), SSL_CTX_free);
    if (SSL_CTX_use_certificate_chain_file(m_ctx.get(), cerf_file.c_str()) != 1) {
        FANG_LOG_ERROR(g_logger) << "SSL_CTX_use_certificate_chain_file("
            << cerf_file << ") error";
        return false;
    }
    if (SSL_CTX_use_PrivateKey_file(m_ctx.get(), key_file.c_str(), SSL_FILETYPE_PEM) != 1) {
        FANG_LOG_ERROR(g_logger) << "SSL_CTX_use_PrivateKey_file("
            << key_file << ") error";
        return false;
    }

    if (SSL_CTX_check_private_key(m_ctx.get()) != 1) {
        FANG_LOG_ERROR(g_logger) << "SSL_CTX_check_private_key cerf_file="
            << cerf_file << " key_file=" << key_file;
        return false;
    }
    return true;
}

SSLSocket::ptr SSLSocket::CreateTCP(fang::Address::ptr address) {
    SSLSocket::ptr sock(new SSLSocket(address->getFamily(), TCP, 0));
    return sock;
}

SSLSocket::ptr SSLSocket::CreateTCPScoket() {
    SSLSocket::ptr sock(new SSLSocket(IPv4, TCP, 0));
    return sock;
}

SSLSocket::ptr SSLSocket::CreateTCPScoket6() {
    SSLSocket::ptr sock(new SSLSocket(IPv6, TCP, 0));
    return sock;
}

std::ostream& SSLSocket::dump(std::ostream& os) const {
    os << "[SSLSocket sock=" << m_sock
        << " is_connected=" << m_isConnected
        << " family=" << m_family
        << " type=" << m_type
        << " protocol=" << m_protocol;
    if (m_localAddress) {
        os << " local_address=" << m_localAddress->toString(); 
    }
    if (m_remoteAddress) {
        os << " remote_address=" << m_remoteAddress->toString();
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Socket& sock) {
    return sock.dump(os);
}

}
