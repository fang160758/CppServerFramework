#include "../http/http_parser.h"
#include <iostream>

const char test_request_data[] = "GET /fang HTTP/1.1\r\n"
                                "host: www.baidu.com\r\n"
                                "Content-Length: 10\r\n\r\n"
                                "0123456789";

const char test_response_data[] = "HTTP/1.1 200 OK\r\n"
        "Date: Tue, 04 Jun 2019 15:43:56 GMT\r\n"
        "Server: Apache\r\n"
        "Last-Modified: Tue, 12 Jan 2010 13:48:00 GMT\r\n"
        "ETag: \"51-47cf7e6ee8400\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 81\r\n"
        "Cache-Control: max-age=86400\r\n"
        "Expires: Wed, 05 Jun 2019 15:43:56 GMT\r\n"
        "Connection: Close\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<html>\r\n"
        "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com/\">\r\n"
        "</html>\r\n";

void test_http_request_parser()
{
    fang::http::HttpRequestParser parse;
    std::string str = test_request_data;
    size_t t = parse.execute(&str[0], str.size());
    std::cout << "execute rt=" << t
              << " has_error=" << parse.hasError()
              << " is_finished=" << parse.isFinished()
              << " Content-length=" << parse.getContentLength()
              << std::endl;

    std::cout << parse.getData()->toString() << std::endl;
}

void test_http_response_parser() {
    fang::http::HttpResponseParser parse;
    std::string str = test_response_data;
    size_t t = parse.execute(&str[0], str.size());
    std::cout << "execute rt=" << t
              << " has_error=" << parse.hasError()
              << " is_finished=" << parse.isFinished()
              << " Content-length=" << parse.getContentLength()
              << std::endl;

    std::cout << parse.getData()->toString();
}

int main()
{
    //test_http_request_parser();
    test_http_response_parser();

}
