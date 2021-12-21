#ifndef __FANG_TCP_SERVER_
#define __FANG_TCP_SERVER_
#include <string>
#include <vector>
#include <memory>
#include "address.h"
#include "socket.h"
#include "iomanager.h"
#include "singleton.h"


namespace fang {


class TcpServer : std::enable_shared_from_this<TcpServer>
                , Noncopyable {

public:
    std::shared_ptr<TcpServer> ptr;

    TcpServer(fang::IOManager* worker = fang::IoManager::GetThis()
            , fang::IOManager* io_worker = fang::IoManager::GetThis()
            , fang::IOManager* accept_worker = fang::IOManager::GetThis());

    ~TcpServer();

    virtual bool bind(fang::Address::ptr addr, bool ssl = false);

    virtual bool bind(const std::vector<fang::Address::ptr>& addrs
            , std::vector<Address::ptr>& fails
            , bool ssl = false);

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

protected:
    std::vector<Socket::ptr> m_socks;
    IOManager* m_worker;
    IOManager* m_ioWorker;
    IOManager* m_acceptWorker;
    uint64_t m_recvTimeout;
    std::string m_name;
    std::string m_type = "tcp";
    bool m_isStop;
    bool m_ssl = false;
}
}






#endif
