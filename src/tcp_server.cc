#include "../inc/tcp_server.h"
#include "../inc/log.h"
#include <functional>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

namespace fang {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");


TcpServer::TcpServer(fang::IOManager* worker
            , fang::IOManager* io_worker
            , fang::IOManager* accept_worker)
        :m_worker(worker)
        ,m_ioWorker(io_worker)
        ,m_acceptWorker(accept_worker)
        ,m_recvTimeout(60 * 1000 *2)
        ,m_name("fang/1.0")
        ,m_isStop(true) {}

TcpServer::~TcpServer() {
    for (auto& i : m_socks) {
        i->close();
    }
    m_socks.clear();
}


bool TcpServer::bind(fang::Address::ptr addr, bool ssl) {
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails, ssl);
}

bool TcpServer::bind(const std::vector<fang::Address::ptr>& addrs
            , std::vector<Address::ptr>& fails
            , bool ssl) {
    m_ssl = ssl;
    for (auto& addr : addrs) {
        Socket::ptr sock = ssl ? SSLSocket::CreateTCP(addr) : Socket::CreateTCP(addr);
        if (!sock->bind(addr)) {
            FANG_LOG_ERROR(g_logger) << "bind fail errno=" << errno
                << "errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        if (!sock->listen()) {
            FANG_LOG_ERROR(g_logger) << "listen fail errno=" << errno
                << " errstr=" << strerror(errno)
                << " addr=[" << addr->toString() << "]";
            fails.push_back(addr);
            continue;
        }
        m_socks.push_back(sock);
    }
    if (!fails.empty()) {
        m_socks.clear();
        return false;
    }
    for (auto& i : m_socks) {
        FANG_LOG_INFO(g_logger) << "type=" << m_type
            << " name=" << m_name
            << " ssl=" << m_ssl
            << " server bind success: " << *i;
    }
    return true;
}

bool TcpServer::start() {
    if (!m_isStop) {
        return true;
    }
    m_isStop = false;
    for (auto& sock : m_socks) {
        m_acceptWorker->schedul(std::bind(&TcpServer::startAccept
                    , shared_from_this(), sock));
    }
    return true;
}

void TcpServer::stop() {
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedul([this, self](){
            for (auto& sock : m_socks) {
            sock->cancelAll();
            sock->close();
            }
            m_socks.clear();
            });
}

std::string TcpServer::toString(const std::string& prefix) {
    std::stringstream ss;
    ss << prefix << "[type=" << m_type
        << " name=" << m_name
        << " worker=" << (m_worker ? m_worker->getName() : "UNKKNOW")
        << " accept=" << (m_acceptWorker ? m_acceptWorker->getName() : "UNKNOW")
        << " recv_timeout=" << m_recvTimeout << "]" << std::endl;
    std::string pfx = prefix.empty() ? "    " : prefix;
    for (auto& i : m_socks) {
        ss << pfx << pfx << *i << std::endl;
    }
    return ss.str();
}
    
bool loadCertificates(const std::string& cerf_file, const std::string& key_file) {
    for (auto& i : m_socks) {
        auto ssl_socket = std::dynamic_pointer_cast<SSLSocket>(i);
        if (ssl_socket) {
            if (!ssl_socket->loadCertificates(cerf_file, key_file)) {
                return false;
            }
        }
    }
    return true;
}

void TcpServer::handleClient(Socket::ptr client) {
    FANG_LOG_INFO(g_logger) << "TcpServer::handleClient: " << *client;
}

void TcpServer::startAccept(Socket::ptr sock) {
    while(!m_isStop) {
        Socket::ptr client = sock->accept();
        if (client) {
            client->setRecvTimeout(m_recvTimeout);
            m_ioWorker->schedul(std::bind(&TcpServer::handleClient
                        , shared_from_this(), client));
        } else {
            FANG_LOG_ERROR(g_logger) << "accept errno=" << errno
                << " errstr=" << strerror(errno);
        }
    }
}
}
