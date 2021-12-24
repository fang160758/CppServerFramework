#pragma once
#include "../stream/socket_stream.h"
#include "../inc/uri.h"
#include "../inc/socket.h"
#include "http.h"
#include <memory>
#include <string>
#include <map>

namespace fang {
namespace http {

struct HttpResult {
    int result;
    HttpResponse::ptr response;
    std::string error;
    
    std::shared_ptr<HttpResult> ptr;

    enum class Error {
        /// 正常
        OK = 0,
        /// 非法URL
        INVALID_URL = 1,
        /// 无法解析HOST
        INVALID_HOST = 2,
        /// 连接失败
        CONNECT_FAIL = 3,
        /// 连接被对端关闭
        SEND_CLOSE_BY_PEER = 4,
        /// 发送请求产生Socket错误
        SEND_SOCKET_ERROR = 5,
        /// 超时
        TIMEOUT = 6,
        /// 创建Socket失败
        CREATE_SOCKET_ERROR = 7,
        /// 从连接池中取连接失败
        POOL_GET_CONNECTION = 8,
        /// 无效的连接
        POOL_INVALID_CONNECTION = 9,
    };

    HttpResult(int _result
            , HttpResponse::ptr _response
            , const std::string& _error)
        :result(_result)
        ,response(_response)
        ,error(_error) {}

    std::string toSting() const;
};

class HttpConnectionPool;

class HttpConnection : public SocketStream {
friend class HttpConnectionPool;
private:
    uint64_t m_createTime = 0;
    uint64_t m_request = 0;

public:
    typedef std::shared_ptr<HttpConnection> ptr;

    // 做HTTP的GET请求 --- > url(string)
    static HttpResult::ptr DoGet(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    
    // 做HTTP的GET请求 --- > uri(class Uri)
    static HttpResult::ptr DoGet(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 做HTTP的POST请求 --- > url(string)
    static HttpResult::ptr DoPost(const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
    
    // 做HTTP的POST请求 --- > uri(class Uri)
    static HttpResult::ptr DoPost(Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
   
    // 做HTTP的请求 --- > url(string)
    static HttpResult::ptr DoRequest(HttpMethod method
                            , const std::string& url
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");
 
    // 做HTTP的请求 --- > uri(class Uri)
    static HttpResult::ptr DoRequest(HttpMethod method
                            , Uri::ptr uri
                            , uint64_t timeout_ms
                            , const std::map<std::string, std::string>& headers = {}
                            , const std::string& body = "");

    // 做HTTP的请求 --- > req(class HttpRequest)
    static HttpResult::ptr DoRequest(HttpRequest::ptr req
                            , Uri::ptr uri
                            , uint64_t timeout_ms);

    HttpConnection(Socket::ptr sock, bool owner = true);

    ~HttpConnection();

    // 接收HTTP响应
    HttpResponse::ptr recvResponse();

    // 发送HTTP请求
    int sendRequest(HttpRequest::ptr req);
};


}
}
