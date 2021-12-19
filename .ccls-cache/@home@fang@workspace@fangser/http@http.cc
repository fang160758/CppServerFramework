#include "http.h"


namespace fang {
namespace http {

    HttpMethod StringTOHttpMethod(const std::string& m) {
    #define XX(num, name, string)\
        if (strcmp(#string, m.c_str()) == 0) {\
            return HttpMethod::name;\
        }
        HTTP_METHOD_MAP(XX);
    #undef XX
        return HttpMethod::INVALID_METHOD;
    }

    HttpMethod CharsToHttpMrthod(const char* m) {
    #define XX(num, name, string)\
        if (strcmp(#string, m) == 0) {\
            return HttpMethod::name;\
        }
        HTTP_METHOD_MAP(XX);
    #undef XX
        return HttpMethod::INVALID_METHOD;
    }
    
    static const char* s_method_string[] = {
    #define XX(num, name, string) #string,
        HTTP_METHOD_MAP(XX)
    #undef XX
    };

    static const uint8_t s_len = sizeof(s_method_string) / sizeof(s_method_string[0]);

    const char* HTTPMethodToString(const HttpMethod& m) {
        uint8_t idx = (uint8_t)m;
        if (idx > s_len) {
            return "<unknow>";
        }
        return s_method_string[idx];
    }

    const char* HttpStatusToString(const HttpStatus& s) {
        switch(s) {
    #define XX(code, name, desc)\
            case HttpStatus::name:\
                return #desc;
            HTTP_STATUS_MAP(XX);
    #undef XX
            default:
                return "<unknow>";
        }
    }

    bool CaseInsensitiveLess::operator()(const std::string& lhs, 
            const std::string& rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }

    
    HttpRequest::HttpRequest(uint8_t version, bool close) 
        : m_method(HttpMethod::GET)
        , m_version(version)
        , m_close(close)
        , m_path("/") {}


    std::string HttpRequest::getHeaders(const std::string& key, const std::string& def) const {
        auto it = m_headers.find(key);
        return it == m_headers.end() ? def : it->second;
    }
    std::string HttpRequest::getParams(const std::string& key,  const std::string& def) const {
        auto it = m_params.find(key);
        return it == m_params.end() ? def : it->second;
    }
    std::string HttpRequest::getCookies(const std::string& key, const std::string& def) const {
        auto it = m_cookies.find(key);
        return it == m_cookies.end() ? def : it->second;
    } 
    
        
            
    bool HttpRequest::hasHeader(const std::string& key, std::string* val) {
        auto it = m_headers.find(key);
        if (it == m_headers.end()) {
            return false;
        }
        if (val) {
            *val = it->second;
        }
        return true;
    }
    bool HttpRequest::hasParam(const std::string& key, std::string* val)  {

        auto it = m_params.find(key);
        if (it == m_params.end()) {
            return false;
        }
        if (val) {
            *val = it->second;
        }
        return true;
    }
    bool HttpRequest::hasCookie(const std::string& key, std::string* val) {

        auto it = m_cookies.find(key);
        if (it == m_cookies.end()) {
            return false;
        }
        if (val) {
            *val = it->second;
        }
        return true;
    }


    std::string HttpRequest::toString() const {
        std::stringstream m_ss;
        dump(m_ss);
        return m_ss.str();
    }

    std::ostream& HttpRequest::dump(std::ostream& os) const {
        os << HTTPMethodToString(m_method) << " " //请求行
           << m_path
           << (m_query.empty() ? "" : "?")
           << m_query
           << (m_fragment.empty() ? "" : "#")
           << m_fragment
           << " HTTP/"
           << ((uint32_t)(m_version >> 4))
           << "."
           << ((uint32_t)(m_version & 0xf))
           << "\r\n";
        os << "connection: " << (m_close ? "close" : "keep-alive") << "\r\n";
        for (auto &i : m_headers) {//请求头
            if (strcasecmp(i.first.c_str(), "connection") == 0) 
                continue;
            os << i.first << ":" << i.second << "\r\n";
        }
        if(!m_body.empty()) {//请求体
            os << "content-length: " << m_body.size() << "\r\n\r\n"
                << m_body;
        } else {
            os << "\r\n";
        }
        return os;
    }

    HttpResponse::HttpResponse(uint8_t version, bool close)
        : m_status(HttpStatus::OK)
        , m_version(version)
        , m_close(close) {}
        
    std::string HttpResponse::getHeader(const std::string& key, const std::string& def) const {
        auto it = m_headers.find(key);
        return (it == m_headers.end() ? def : it->second); 
    }

    std::string HttpResponse::toString() const {
        std::stringstream m_ss;
        dump(m_ss);
        return m_ss.str();
    }

    std::ostream& HttpResponse::dump(std::ostream& os) const {
        os << "HTTP/"
           << ((uint32_t)(m_version >> 4))
           << "."
           << ((uint32_t)(m_version & 0xf))
           << " "
           << (uint32_t)m_status
           << " "
           << (m_reason.empty() ? HttpStatusToString(m_status) : m_reason)
           << "\r\n";
        
        for (auto& i : m_headers) {
            if(strcasecmp(i.first.c_str(), "connection") == 0) {
                continue;
            }
            os << i.first << ":" << i.second << "\r\n";
        }
        for (auto& i : m_cookies) {
            os << "Set-Cookie:" << i << "\r\n";
        }

        if (!m_body.empty()) {
            os << "Content-Length:" << m_body.size() << "\r\n\r\n" 
                << m_body;
        } else {
            os << "\r\n";
        }
        return os;
    }
}
}
