#include "http_connection.h"
#include "../inc/log.h"
#include "http_parser.h"
#include <memory>
#include <string>
#include <sstream>

namespace fang {
namespace http {

static fang::Logger::ptr g_logger = FANG_LOG_NAME("system");

std::string HttpResult::toSting() const {
    std::stringstream ss;
    ss << "[HttpResult result=" << result
       << " error=" << error
       << " response=" << (response ? response->toString() : "nullptr")
       << "]";
    return ss.str();
}

HttpConnection::HttpConnection(Socket::ptr sock, bool owner)
    :SocketStream(sock, owner) {}


HttpConnection::~HttpConnection() {
    FANG_LOG_DEBUG(g_logger) << "HttpConnection::~HttpConnection";
}

HttpResponse::ptr HttpConnection::recvResponse() {
    HttpResponseParser::ptr parser(new HttpResponseParser);
    uint64_t buff_size = HttpRequestParesr::GetHttpRequestBufferSize();
    std::shared_ptr<char> buffer(
            new char[buff_size + 1], [](char* ptr) {
                delete[] ptr;
            });

    char* data = buffer.get();

    int offset = 0;

    do { // 解析sock接收到的响应包
        int len = read(data + offset, buff_size - offset);
        if (len <= 0) {
            close();
            return nullptr;
        }
        len += offset;
        data[len] = '\0';
        size_t nparse = parser->execute(data, len, false);
        if (nparse->hasError()) {
            close();
            return nullptr;
        }
        offset = len - nparse;
        if (offset == (int) buff_size) {
            close();
            return nullptr;
        }
        if (parser->isFinished()) {
            break;
        }
    } while(true);
    
    auto& client_parser = parser->getParser();
    std::string body;

    if (client_parser.chunked) {
        int len = offset;
        do {
            bool begin = true;
            do {
                if (!begin || len == 0) {
                    int rt = read(data + len, buff_size - len);
                    if (rt <= 0) {
                        close();
                        return nullptr;
                    }
                }

                data[len] = '\0';
                size_t nparse = parser->execute(data, len, true);
                if (parser->hasError()) {
                    close();
                    return nullptr;
                }
                len -= nparse;
                if (len == (int)buff_size) {
                    close();
                    return nullptr;
                }
                begin = false;
            } while(parser->isFinished());

            FANG_LOG_DEBUG(g_logger) << "content_len=" << client_parser.content_len;

            if (client_parser.content_len + 2 <= len) {
                body.append(data, client_parser.content_len);
                memmove(data, data + client_parser.content_len + 2
                        , len - client_parser.content_len - 2);
                len -= client_parser.content_len + 2;
            } else {
                body.append(data, len);
                int left = client_parser.content_len - len + 2;
                while(left > 0) {
                    int rt = read(data, left > (int)buff_size ? (int)buff_size : left);
                    if (rt <= 0) {
                        close();
                        return nullptr;
                    }
                    body.append(data, rt);
                    left -= rt;
                }
                body.resize(body.size() - 2);
                len = 0;
            }
        } while(!client_parser.chunks_done);
    } else {
        int64_t length = parser->getContentLength();
        if (length >= offset) {
            memcpy(&body[0], data, offset);
            len = offset;
        } else {
            memcpy(&body[0], data, length);
            len = length;
        }
        length -= offset;
        if (length > 0) {
            if (readFixSize(&body[len], length) <= 0) {
                close();
                return nullptr;
            }
        }
    }
    if (!body.empty()) {
        auto content_encoding = parser->getData()->getHeader("content-encoding");

        FANG_LOG_DEBUG(g_logger) << "content_encoding: " << content_encoding
            << " size=" << body.size();

        if (strncasecmp(content_encoding.c_str(), "gzip") == 0) {
            auto zs = 
        }
    }

}






}
}
