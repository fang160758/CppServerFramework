#include "../inc/tcp_server.h"
#include "../inc/log.h"
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
        Socket::ptr sock = ssl ? SS
    }
}

    bool loadCertificates(const std::string& cerf_file, const std::string& key_file);


    virtual bool start();

    virtual void stop();

    uint64_t getRecvTimeout() const { return m_recvTimeout; }

    std::string getName() const { return m_name; }

    void setRecvTimeout(uint64_t v) { m_recvTimeout = v; }

    virtual void setName(const std::string& v) { m_name = v; }

    bool isStop() const { return m_isStop; };

    virtual std::string toString(const std::string& prefix = "");
    
    std::vector<Socket::ptr> getSocks const { return m_socks; }

protected:
    virtual void handleClient(Socket::ptr client);

    virtual void startAccept(Socket::ptr sock);


}
