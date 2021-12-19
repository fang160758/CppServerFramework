#include "../http/http.h"
#include <iostream>

void test_request() {
    fang::http::HttpRequest::ptr req(new fang::http::HttpRequest(0x11, true));
    req->setHeader("host", "www.baidu.com");
    req->setBody("hello fang");
    req->dump(std::cout) << std::endl;
}

void test_response() {
    fang::http::HttpResponse::ptr req(new fang::http::HttpResponse());
    req->setHeader("X-X","fang");
    req->setBody("hello fang");
    req->setStatus((fang::http::HttpStatus)400);
    req->setClose(false);
    req->dump(std::cout) << std::endl;
}

int main()
{
    test_request();
    test_response();
}
