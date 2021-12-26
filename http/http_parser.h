#ifndef __FANG_HTTP_HTTP_PARSER_H__
#define __FANG_HTTP_HTTP_PARSER_H__

#include <memory>
#include <ostream>
#include "http.h"
#include "http11_parser.h"
#include "httpclient_parser.h"


namespace fang {
namespace http {
    class HttpRequestParser {
        public:
            typedef std::shared_ptr<HttpRequestParser> ptr;
            HttpRequestParser();

            size_t execute(char* data, size_t len);
            int isFinished();
            int hasError();

            HttpRequest::ptr getData() const { return m_data; }
            void setError(int error) { m_error = error; }
            const http_parser& getParams() const { return m_parser; } 

            uint64_t getContentLength() const {
                return m_data->getHeaderAs<uint64_t>("content-length",0);
            }

            static uint64_t GetHttpResquestBufferSize();
            static uint64_t GetHttpResquestMaxBodySize();
            static uint64_t GetHttpResponseBufferSize();
            static uint64_t GetHttpResponseMaxBodySize();

        private:
            http_parser m_parser;
            HttpRequest::ptr m_data;
            int m_error;
    };

    class HttpResponseParser {
        public:
            typedef std::shared_ptr<HttpResponseParser> ptr;
            HttpResponseParser();

            size_t execute(char* data, size_t len, bool chunck);
            int isFinished();
            int hasError();

            HttpResponse::ptr getData() const { return m_data; }
            const httpclient_parser& getParser() const { return m_parser; }
            void setError(int v) { m_error = v; }
            
            uint64_t getContentLength() const {
                return m_data->getHeaderAs<uint64_t>("content-length",0);
            }
        private:
            httpclient_parser m_parser;
            HttpResponse::ptr m_data;
            int m_error; 
    };

    std::ostream& operator<<(std::ostream& os, const HttpRequest& req);
    std::ostream& operator<<(std::ostream& os, const HttpResponse& rsp);


}
}

#endif
