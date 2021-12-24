#pragma once
#include <memory>
#include <string>
#include <stdint.h>
#include <type_traits>
#include "address.h"

namespace fang {

/* URL的构成：
 * scheme://host:port/path?query#fragment:
 * Scheme: 通信协议，一般为http、https等
 * Host: 服务器的域名主机名或ip地址
 * Port: 端口号，此项为可选项，默认为80
 * Path: 目录，由“/”隔开的字符串，表示的是主机上的目录或文件地址
 * Query: 查询，此项为可选项，可以给动态网页传递参数，用“&”隔开，每个参数的名和值用“=”隔开
 * Fragment: 信息片段，字符串，用于指定网络资源中的某片断
 */

// URI封装
class Uri {
private:
    std::string m_scheme;
    std::string m_userinfo;
    std::string m_host;
    std::string m_path;
    std::string m_query;
    std::string m_fragment;
    int32_t m_port;

public:   
    typedef std::shared_ptr<Uri> ptr;

    // 创建一个Uri 对象
    static Uri::ptr Create(const std::string& uristr);

    Uri();

    const std::string& getScheme()   const { return m_scheme; }
    const std::string& getUserinfo() const { return m_userinfo; }
    const std::string& getHost()     const { return m_host; }
    const std::string& getQuery()    const { return m_query; }
    const std::string& getFragment() const { return m_fragment; }
    const std::string& getPath() const;
    int32_t getPort() const;
    
    void setScheme(  const std::string& v) { m_scheme = v; }   
    void setUserinfo(const std::string& v) { m_userinfo = v; }
    void setHost(    const std::string& v) { m_host = v; }
    void setPath(    const std::string& v) { m_path = v; }
    void setQuery(   const std::string& v) { m_query = v; }
    void setFragment(const std::string& v) { m_fragment = v; }
    void setPort(int32_t v) { m_port = v; }


    std::ostream& dump(std::ostream& os) const;

    std::string toString() const;

    Address::ptr createAddress() const;

private:
    bool isDefaultPort() const;
};

}
