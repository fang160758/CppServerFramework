#pragma once

#include "http.h"
#include "../stream/socket_stream.h"
#include <memory>

namespace fang {
namespace http {
    class HttpSession : public SocketStream {
        typedef std::shared_ptr<HttpSession> ptr;
        HttpSession(Socket::ptr sock, bool owner = true);

        HttpRequest::ptr recvRequest();

        int sendResponse(HttpResponse::ptr rsp);

    };
}
   
}
