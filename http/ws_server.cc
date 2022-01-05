#include "ws_server.h"
#include "../inc/log.h"

namespace fang {
namespace http{
static fang::Logger::ptr g_logger = fang::LOG_FANG_NAME("system");

WSServer::WSServer(fang::IoManager* worker = fang::IoManager::GetThis()
       , fang::IoManager* io_worker = fang::IoManager::GetThis()
       , fang::IoManager* accept_worker = fang::IoManager::GetThis())
    : TcpServer(worker, io_worker, accept_worker) {
        m_dispatch.reset(new WSServletDispatch);
        m_type = "websocket_server";
    }


void WSServer::handleClient(Socket::ptr client) {
    fang::FANG_LOG_INFO(g_logger) << "WSServer::handleClient" << *client;
    WSSession::ptr session(new WSSession(client));
    do {
        HttpRequest::ptr header = session->handleShake();
        if (!header) {
            FANG_LOG_DEBUG(g_logger) << "handleShake error";
            break;
        }

        WSServlet::ptr servlet = m_dispatch->getWSServlet(header->getPath());
        if (!servlet) {
            FANG_LOG_DEBUG(g_logger) << "no match WSServlet";
            break;
        }
        int rt = servlet->onConnect(header, session);
        if (rt) {
            FANG_LOG_DEBUG(g_logger) << "onConnect return " << rt;
            break;
        }
        while(true) {
            auto msg = session->recvMessage();
            if (!msg) {
                break;
            } 
            rt = servlet->handle(header, msg, session);
            if (rt) {
                FANG_LOG_DEBUG(g_logger) << "handle return " << rt;
                break;
            }
        }
        servlet->onClose(header, session);
    } while(0);
    session->close();
}

}
}
