#pragma once
#include "../inc/tcp_server.h"
#include "http_session.h"

namespace fang {
namespace http {
class HttpServer: public TcpServer {
public:
    typedef std::shared_ptr<HttpServer> ptr;

    HttpServer(bool keepalive = false
        , fang::IoManager* worker = fang::IoManager::GetThis()
        , fang::IoManager* io_worker = fang::IoManager::GetThis()
        , fang::IoManager* accept_worker = fang::IoManager::GetThis());



protected:
    virtual void handleClient(Socket::ptr client) override;
private:
    bool m_isKeepAlive;
}
}
}
