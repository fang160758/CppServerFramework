#ifndef __FANG_HTTP_WS_SERVLET_H__
#define __FANG_HTTP_WS_SERVLET_H__

#include "http.h"
#include "servlet.h"
#include "ws_session.h"
#include <functional>
#include <memory>
#include <string>


namespace fang {
namespace http{

class WSServlet : public Servlet {
public:
    typedef std::shared_ptr<WSServer> ptr;
    WSServlet(const std::string& name)
        : Servlet(name) {}

    virtual ~WSServlet() {}

    virtual int32_t handle(fang::http::HttpRequest::ptr resquest
                    , fang::http::HttpResponse::ptr response
                    , fang::http::HttpSession::ptr session) override {
        return 0;
    }

    virtual int32_t onConnect(fang::http::HttpRequest::ptr header
            , fang::http::WSSession::ptr session) = 0;

    virtual int32_t onClose(fang::http::HttpRequest::ptr header
            , fang::http::WSSession::ptr session) = 0;
    
    virtual int32_t handle(fang::http::HttpRequest::ptr header
            , fang::http::WSFrameMessage::ptr msg
            , fang::http::WSSession::ptr session) = 0;
};

class FunctionWSServlet : public WSServlet {
public:
    typedef std::shared_ptr<FunctionWSServlet> ptr;
    typedef std::function<int32_t(fang::http::HttpRequest::ptr header
            , fang::http::WSSession::ptr session)> on_connect_cb;
    typedef std::function<int32_t(fang::http::HttpRequest::ptr header
            , fang::http::WSSession::ptr session)> on_close_cb;
    typedef std::function<int32_t(fang::http::HttpRequest::ptr header
            , fang::http::WSFrameMessage::ptr msg
            , fang::http::WSSession::ptr session)> callback;

    FunctionWSServlet(callback cb
            , on_connect_cb connect_cb = nullptr
            , on_close_cb close_cb = nullptr);

    virtual int32_t onConnect(fang::http::HttpRequest::ptr header
            , fang::http::WSSession::ptr session) override;

    virtual int32_t onClose(fang::http::HttpRequest::ptr header
            , fang::http::WSSession::ptr session) override;
    
    virtual int32_t handle(fang::http::HttpRequest::ptr header
            , fang::http::WSFrameMessage::ptr msg
            , fang::http::WSSession::ptr session) override;

protected:
    callback m_callback;
    on_connect_cb m_onConnect;
    on_close_cb m_onClose;
};


class WSServletDispatch : public ServletDispatch {
public:
    typedef std::shared_ptr<WSServletDispatch> ptr;
    typedef RWMutex RWMutexType;

    WSServletDispatch();
    
    void addServlet(const std::string& uri
            , FunctionWSServlet::callback cb
            , FunctionWSServlet::on_connect_cb connect_cb = nullptr
            , FunctionWSServlet::on_close_cb close_cb = nullptr);

    void addGlobServlet(const std::string& uri
            , FunctionWSServlet::callback cb
            , FunctionWSServlet::on_connect_cb connect_cb = nullptr
            , FunctionWSServlet::on_close_cb close_cb = nullptr);
    WSServlet::ptr getWSServlet(const std::string& uri);
};

}
}
#endif
