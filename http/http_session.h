#pragma once

#include "http.h"
#include "../stream/socket_stream.h"
#include <memory>

namespace fang {
namespace http {
class HttpSession : public SocketStream {
 
    public:
    typedef std::shared_ptr<HttpSession> ptr;
    HttpSession(Socket::ptr sock, bool owner = true);

    HttpRequest::ptr recvRequest(); // 接收HTTP请求

    int sendResponse(HttpResponse::ptr rsp); // 发送HTTP响应

};
}

}
