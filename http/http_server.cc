#include "http_server.h"
#include "../inc/log.h"

namespace fang {
namespace http {    
    static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");


    HttpServer::HttpServer(bool keepalive = false
        , fang::IoManager* worker
        , fang::IoManager* accept_worker) 
    :TcpServer(worker, accept_worker)
    :m_isKeepAlive(keepalive) {}

    void HttpServer::handleClient(Socket::ptr client) {
        HttpSession::ptr session(new HttpSession(client));
        do {
            auto req = session->recvRequest();
            if (!req) {
                FANG_LOG_WARN(g_logger) << "recv http request fail, errno="
                    << errno << " strerr=" << strerror(errno)
                    << " client:" << client;
                break;
            }
            HttpResponse::ptr rsp(req->getVersion(), req->isClose() || !m_isKeepAlive);
            rsp->setBody("hello fang");
            session->sendResponse(rsp);
        }while(true);        
    }

    HttpServer::HttpServer()
}
}
