#include "http_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


namespace fang {
namespace http {
        
    static uint64_t s_http_request_buffer_size = 0;
    static uint64_t s_http_request_max_body_size = 0;
    static uint64_t s_http_response_buffer_size = 0;
    static uint64_t s_http_response_max_body_size = 0;

    
    uint64_t HttpRequestParser::GetHttpResquestBufferSize() {
        return s_http_request_buffer_size;
    }
    uint64_t HttpRequestParser::GetHttpResquestMaxBodySize() {
        return s_http_request_max_body_size;
    }
    uint64_t HttpRequestParser::GetHttpResponseBufferSize() {
        return s_http_response_buffer_size;
    }
    uint64_t HttpRequestParser::GetHttpResponseMaxBodySize() {
        return s_http_response_max_body_size;
    }



    void on_request_method(void *data, const char *at, size_t length) {
        HttpRequestParser * req = static_cast<HttpRequestParser*>(data);
        HttpMethod m = CharsToHttpMrthod(at);
        if (m == HttpMethod::INVALID_METHOD) {
            perror("invalid http method");
            return ;
        }
        req->getData()->setMethod(m);
    }

    void on_request_uri(void *data, const char *at, size_t length) {
        
    }

    void on_request_fragment(void *data, const char *at, size_t length) {
        HttpRequestParser* req = static_cast<HttpRequestParser*>(data);
        req->getData()->setFragment(std::string(at, length));
    }

    void on_request_path(void *data, const char *at, size_t length) {
        HttpRequestParser* req = static_cast<HttpRequestParser*>(data);
        req->getData()->setPath(std::string(at, length));
    }

    void on_request_query(void *data, const char *at, size_t length) {
        HttpRequestParser* req = static_cast<HttpRequestParser*>(data);
        req->getData()->setQuery(std::string(at, length));
    }

    void on_request_version(void *data, const char *at, size_t length) {
        HttpRequestParser* req = static_cast<HttpRequestParser*>(data);
        uint8_t version = 0;
        if (strncasecmp(at, "HTTP/1.1", length) == 0) {
            version = 0x11;
        } else if (strncasecmp(at, "HTTP/1.0", length) == 0) {
            version = 0x10;
        } else {
            perror("invalid http request version");
            return;
        }
        req->getData()->setVersion(version);
    }

    void on_request_header_done(void *data, const char *at, size_t length) {
        
    }

    void on_request_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {
        HttpRequestParser* req = static_cast<HttpRequestParser*>(data);
        if (flen == 0) {
            perror("invalid http request field length == 0");
            return;
        }
        req->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
    }
    
    size_t HttpRequestParser::execute(char* data, size_t len) {
        size_t offset = http_parser_execute(&m_parser, data, len, 0);
        std::cout << offset << "\t" << len << std::endl;
        memmove(data, data + offset, len - offset);
        return offset;
    }

    int HttpRequestParser::isFinished() {
        return http_parser_is_finished(&m_parser);
    }

    int HttpRequestParser::hasError() {
        return (m_error || http_parser_has_error(&m_parser));
    }


    HttpRequestParser::HttpRequestParser() 
        : m_error(0) {
        http_parser_init(&m_parser);
        m_data.reset(new HttpRequest);
        m_parser.request_method = on_request_method;
        m_parser.request_uri = on_request_uri;
        m_parser.fragment = on_request_fragment;
        m_parser.request_path = on_request_path;
        m_parser.query_string = on_request_query;
        m_parser.http_version = on_request_version;
        m_parser.header_done = on_request_header_done;
        m_parser.http_field = on_request_http_field;
        m_parser.data = this;
               
    }


    void on_response_reason(void *data, const char *at, size_t length) {
        HttpResponseParser* rsp = static_cast<HttpResponseParser*>(data);
        rsp->getData()->setReason(std::string(at, length));
    }

    void on_response_status(void *data, const char *at, size_t length) {
        HttpResponseParser* rsp = static_cast<HttpResponseParser*>(data);
        HttpStatus status = (HttpStatus)(atoi(at));
        rsp->getData()->setStatus(status);
    }

    void on_response_chunk(void *data, const char *at, size_t length) {
    }

    void on_response_version(void *data, const char *at, size_t length) {
        HttpResponseParser* rsp = static_cast<HttpResponseParser*>(data);
        uint8_t version = 0;
        if (strncasecmp(at, "HTTP/1.1", length) == 0) {
            version = 0x11;
        } else if (strncasecmp(at, "HTTP/1.0", length) == 0) {
            version = 0x10;
        } else {
            perror("invalid http request version");
            return;
        }
        //std::cout << "DEBUG_1" << std::endl;
        rsp->getData()->setVersion(version);
        //std::cout << "DEBUG_2" << std::endl;
    }

    void on_response_header_done(void *data, const char *at, size_t length) {

    }

    void on_response_last_chunk(void *data, const char *at, size_t length) {

    }

    void on_response_http_field(void *data, const char *field, size_t flen, const char *value, size_t vlen) {
        HttpResponseParser* rsp = static_cast<HttpResponseParser*>(data);
        if (flen == 0) {
            perror("invalid http response field length == 0");
            return ;
        }
        rsp->getData()->setHeader(std::string(field, flen), std::string(value, vlen));
        //std::cout << "DEBUG_3" << std::endl;
    }
    size_t HttpResponseParser::execute(char* data, size_t len) {
        size_t offset = httpclient_parser_execute(&m_parser, data, len, 0);
        //std::cout << "DEBUG_4" << std::endl;
        //std::cout << offset << '\t' << len << std::endl;
        //如果返回的offest > len 发生段错误。
        memmove(data, data + offset, len - offset);
        return offset;
    }

    int HttpResponseParser::isFinished() {
        return httpclient_parser_is_finished(&m_parser);
    }

    int HttpResponseParser::hasError() {
        return (m_error || httpclient_parser_has_error(&m_parser));
    }


    HttpResponseParser::HttpResponseParser()
        : m_error(0) {
        httpclient_parser_init(&m_parser);
        m_data.reset(new HttpResponse);
        m_parser.reason_phrase = on_response_reason;
        m_parser.status_code   = on_response_status;
        m_parser.chunk_size    = on_response_chunk;
        m_parser.http_version  = on_response_version;
        m_parser.header_done   = on_response_header_done;
        m_parser.last_chunk    = on_response_last_chunk;
        m_parser.http_field    = on_response_http_field;
        m_parser.data = this;
    }
    std::ostream& operator<<(std::ostream& os, const HttpRequest& req) {
        return req.dump(os);
    }
    std::ostream& operator<<(std::ostream& os, const HttpResponse& rsp) {
        return rsp.dump(os);
    }
}
}
