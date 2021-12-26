#pragma once
#include "../stream/socket_stream.h"
#include "../inc/uri.h"
#include "../inc/socket.h"
#include "../inc/mutex.h"
#include "http.h"
#include <memory>
#include <string>
#include <map>
#include <list>


namespace fang {
namespace http {

struct HttpResult {
    int result;
    HttpResponse::ptr response;
    std::string error;
    
    typedef std::shared_ptr<HttpResult> ptr;

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
    int sendRequest(HttpRequest::ptr rsp);
};



class HttpConnectionPool {

public:
    typedef std::shared_ptr<HttpConnectionPool> ptr;
    typedef Mutex MutexType;

    static HttpConnectionPool::ptr Create(const std::string& uri
                                    , const std::string& vhost
                                    , uint32_t max_size
                                    , uint32_t max_alive_time
                                    , uint32_t max_request);

    HttpConnectionPool(const std::string& host
                , const std::string& vhost
                , uint32_t port
                , bool is_https
                , uint32_t max_size
                , uint32_t max_alive_time
                , uint32_t max_request);

    HttpConnection::ptr getConnection();

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

private:
    static void ReleasePtr(HttpConnection* ptr, HttpConnectionPool* pool);


private:
    std::string m_host;
    std::string m_vhost;
    uint32_t m_port;
    uint32_t m_maxSize;
    uint32_t m_maxAliveTime;
    uint32_t m_maxRequest;
    bool m_isHttps;

    MutexType m_mutex;
    std::list<HttpConnection*> m_conns;
    std::atomic<int32_t> m_total = {0};

};


}
}
