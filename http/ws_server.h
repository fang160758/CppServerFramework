#pragma once 
#include "../inc/tcp_server.h"
#include <memory>


namespace fang {
namespace http {

class WSServer : public TcpServer {
public:
    typedef std::shared_ptr<WSServer> ptr;
    WSServer(fang::IoManager* worker = fang::IoManager::GetThis()
           , fang::IoManager* io_worker = fang::IoManager::GetThis()
           , fang::IoManager* accept_worker = fang::IoManager::GetThis());

    WSServer::ptr getWSServletDispatch() const { return m_dispatch; }
    void setWSServletDispatch(WSServletDispatch::ptr v) { m_dispatch = v; }

protected:
    virtual void handleClient(Socket::ptr client) override;

private:
    WSServletDispatch::ptr m_dispatch;

};


}
}
