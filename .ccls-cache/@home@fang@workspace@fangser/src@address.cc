#include "../inc/address.h"
#include "../inc/endian_.h"
#include <sstream>
#include <netdb.h> 
#include <ifaddrs.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

namespace fang {
    
    template <typename T>
    static T CreateMask(uint32_t bits) {
        return (1 << ((sizeof(T) * 8 - bits))) - 1;
    }
    
    template <typename T>
    static uint32_t CountBytes(T value) {
        uint32_t result = 0;
        for (; value; ++result) {
            value &= value - 1;
        }
        return result;
    }

    Address::ptr Address::Create(const struct sockaddr* addr, socklen_t addrlen) {
        if (addr == nullptr) {
            return  nullptr;
        }
        Address::ptr result;
        switch(addr->sa_family) {
            case AF_INET:
                result.reset(new IPv4Address(*(const struct sockaddr_in*)addr));
                break;
            case AF_INET6:
                result.reset(new IPv6Address(*(const struct sockaddr_in6*)addr));
                break;
            default:
                result.reset(new UnknowAddress(*addr));
                break;
        }
        return result;
    }

    bool Address::Lookup(std::vector<Address::ptr>& result, const std::string& host,
            int family, int type, int protocol) {
        struct addrinfo hints, *results, *next;
        memset(&hints, 0, sizeof(hints));
        hints.ai_flags = 0;
        hints.ai_family = family;
        hints.ai_socktype = type;
        hints.ai_protocol = protocol;
        hints.ai_addrlen = 0;
        hints.ai_canonname = NULL;
        hints.ai_addr = NULL;
        hints.ai_next = NULL;

        std::string node;
        const char* service = NULL;

        if (!host.empty() && host[0] == '[') {
            const char* endipv6 = (const char*) memchr(host.c_str() + 1, ']', host.size() - 1);
            if (endipv6) {
                if (*(endipv6 + 2) == ':') {
                    service = endipv6 + 2;
                }
                node = host.substr(1, endipv6 - host.c_str() - 1);
            }
        }

        if (node.empty()) {
            service = (const char*)memchr(host.c_str(), ':', host.size());
            if (service) {
                if (!memchr(service +1, ':', host.c_str() + host.size() - service - 1)) {
                    node = host.substr(0, service - host.c_str());
                    ++service;
                }
            }
        }

        if (node.empty()) {
            node = host;
        }
        
        int error = getaddrinfo(node.c_str(), service, &hints, &results);
        if (error != 0) {
            throw std::logic_error("getaddrinfo fail!");
        }
        next = results;
        while(next) {
            result.push_back(Create(next->ai_addr,(socklen_t)next->ai_addrlen));
            next = next->ai_next;
        }

        freeaddrinfo(results);
        return !result.empty();
    }

    Address::ptr Address::LookupAny(const std::string& host, 
            int family, int type, int protocol) {
        std::vector<Address::ptr> result;
        if (Lookup(result, host, family, type, protocol)) {
            return result[0];
        }
        return nullptr;
    }
    IPAddress::ptr Address::LookupAnyIPAddress(const std::string& host,
            int family, int type, int protocol) {
        std::vector<Address::ptr> result;
        if (Lookup(result, host, family, type, protocol)) {
            for (auto&i : result) {
                IPAddress::ptr v = std::dynamic_pointer_cast<IPAddress>(i);
                if (v) {
                    return v;
                }
            }
        }
        return nullptr;
    }
    //获取网卡的地址信息
    bool Address::GetInterfaceAddresses(std::multimap<std::string, 
            std::pair<Address::ptr, uint32_t> >& result, int family) {
        struct ifaddrs *next, *results;
        if (getifaddrs(&results) == -1) {
            perror("getifaddrs error");
            return false;
        }
        try {
            for (next = results; next; next = next->ifa_next) {
                Address::ptr addr;
                uint32_t prefix_len = ~0u;
                if (family != AF_UNSPEC && family != next->ifa_addr->sa_family) {
                    continue;
                }
                switch (next->ifa_addr->sa_family) {
                    case AF_INET:
                    {
                        addr = Create(next->ifa_addr, (socklen_t)sizeof(sockaddr_in));
                        uint32_t netmask = ((struct sockaddr_in*)next->ifa_netmask)->sin_addr.s_addr;
                        prefix_len = CountBytes(netmask);
                    }
                        break;
                    case AF_INET6:
                    {
                        addr = Create(next->ifa_addr, (socklen_t)sizeof(sockaddr_in6));
                        in6_addr& netmask = ((sockaddr_in6*)next->ifa_netmask)->sin6_addr;
                        for (int i = 0; i < 16; i++) {
                            prefix_len += CountBytes(netmask.s6_addr[i]);
                        }
                    }
                        break;
                    default:
                        break;
                }
                if (addr) {
                    result.insert(std::make_pair(next->ifa_name, make_pair(addr, prefix_len)));
                }
            }
        } catch (...) {
            perror("GetInterfaceAddresses error");
            freeifaddrs(results);
            return false;
        }
        freeifaddrs(results);
        return true;

    }

    //获取指定网卡的地址信息
    bool Address::GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t> >& result, 
            const std::string& iface, int family) {

        if(iface.empty() || iface == "*") {
            //todo something
            return true;
        }
        std::multimap<std::string, std::pair<Address::ptr, uint32_t> > results;
        if (!GetInterfaceAddresses(results, family)) {
            return false;
        }

        auto its = results.equal_range(iface);
        for (;its.first != its.second; ++its.first) {
            result.push_back(its.first->second);
        }
        return true;
    }
          

    int Address::getFamily() const {
        return getAddr()->sa_family;
    }
    
    std::string Address::toString() {
        std::stringstream ss;
        insert(ss);
        return ss.str();
    }

    bool Address::operator<(const Address& rhs) const {
        socklen_t minlen = std::min(getAddrLen(), rhs.getAddrLen());
        int result = memcmp(getAddr(), rhs.getAddr(), minlen);
        if (result < 0) {
            return true;
        } else if (result > 0) {
            return false;
        } else if (getAddrLen() < rhs.getAddrLen()) {
            return true;
        } else {
            return false;
        }
    }

    bool Address::operator==(const Address& rhs) const {
        return getAddrLen() == rhs.getAddrLen() &&
                memcmp(getAddr(), rhs.getAddr(), getAddrLen()) == 0;
    }

    bool Address::operator!=(const Address& rhs) const {
        return !(*this == rhs);
    }

    IPv4Address::IPv4Address(const struct sockaddr_in& address) {
        m_addr = address;
    }

    IPv4Address::IPv4Address(uint32_t address, uint16_t port) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = byteswapOnLittleEndian(port);
        m_addr.sin_addr.s_addr = byteswapOnLittleEndian(address);

    }

    sockaddr* IPv4Address::getAddr() {
        return (sockaddr*)&m_addr;
    }
    
    const sockaddr* IPv4Address::getAddr() const {
        return (sockaddr*)&m_addr;
    }

    socklen_t IPv4Address::getAddrLen() const {
        return sizeof(m_addr);
    }
    
    std::ostream& IPv4Address::insert(std::ostream& os) const {
        uint32_t addr = byteswapOnLittleEndian(m_addr.sin_addr.s_addr);
        os << ((addr >> 24) & 0xff) << "."
           << ((addr >> 16) & 0xff) << "."
           << ((addr >> 8) & 0xff) <<"."
           << ((addr) * 0xff);
        os << ":" << byteswapOnLittleEndian(m_addr.sin_port);
        return os;
    }

    IPAddress::ptr IPv4Address::boradcastAddress(uint32_t prefix_len) {
        if (prefix_len > 32) {
            return nullptr;
        }
        sockaddr_in baddr(m_addr);
        baddr.sin_addr.s_addr |= byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
        return IPv4Address::ptr(new IPv4Address(baddr));
    }

    IPAddress::ptr IPv4Address::networkAddress(uint32_t prefix_len) {
        if (prefix_len > 32) {
            return nullptr;
        }

        struct sockaddr_in baddr(m_addr);
        baddr.sin_addr.s_addr &= byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
        return IPv4Address::ptr(new IPv4Address(baddr));
    }

    IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
        struct sockaddr_in subnet;
        memset(&subnet, 0, sizeof(subnet));
        subnet.sin_family = AF_INET;
        subnet.sin_addr.s_addr = ~byteswapOnLittleEndian(CreateMask<uint32_t>(prefix_len));
        return IPv4Address::ptr(new IPv4Address(subnet));
    }

    IPv4Address::ptr IPv4Address::Create(const char* address, uint16_t port) {
        IPv4Address::ptr rt(new IPv4Address);
        rt->m_addr.sin_family = AF_INET;
        rt->m_addr.sin_port = byteswapOnLittleEndian(port);
        int result = inet_pton(AF_INET, address, &rt->m_addr.sin_addr);
        if (result < 0) {
            throw std::logic_error("inet_pton failed!");
        }
        return rt;
    }
    
    uint32_t IPv4Address::getPort() const {
        return byteswapOnLittleEndian(m_addr.sin_port);
    }

    void IPv4Address::setPort(uint16_t port) {
        m_addr.sin_port = byteswapOnLittleEndian(port);
    }


    IPv6Address::IPv6Address() {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin6_family = AF_INET6;
    }
    IPv6Address::IPv6Address(const struct sockaddr_in6& address) {
        m_addr = address;
    }

    IPv6Address::IPv6Address(const uint8_t address[16], uint16_t port) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin6_family = AF_INET6;
        m_addr.sin6_port = byteswapOnLittleEndian(port);
        memcpy(&m_addr.sin6_addr.s6_addr, address, 16);
    }

    IPv6Address::ptr IPv6Address::Create(const char* address, uint16_t port) {
        IPv6Address::ptr rt(new IPv6Address);
        rt->m_addr.sin6_port = byteswapOnLittleEndian(port);
        int result = inet_pton(AF_INET6, address, &rt->m_addr.sin6_addr);
        if (result < 0) {
            throw std::logic_error("inet_pton fail!");
        }
        return rt;
    }
    sockaddr* IPv6Address::getAddr() {
        return (sockaddr*)&m_addr;
    }
    const sockaddr* IPv6Address::getAddr() const {
        return (sockaddr*)&m_addr;
    }

    socklen_t IPv6Address::getAddrLen() const {
        return sizeof(m_addr);
    }
    
    std::ostream& IPv6Address::insert(std::ostream& os) const {
        os << "[";
        uint16_t* addr = (uint16_t*)m_addr.sin6_addr.s6_addr;
        bool user_zeros = false;
        for (int i = 0; i < 8; ++i) {
            if (addr[i] == 0  && !user_zeros) {
                continue;
            }
            if (i && addr[i - 1] == 0 &&!user_zeros) {
                os << ":";
                user_zeros = true;
            }
            if (i) {
                os << ":";
            }
            os << std::hex << (unsigned int)byteswapOnLittleEndian(addr[i]) << std::dec;
        }

        if (!user_zeros && addr[7] == 0) {
            os << "::";
        }
        os << "]:" << byteswapOnLittleEndian(m_addr.sin6_port);
        return os;
    }

    IPAddress::ptr IPv6Address::boradcastAddress(uint32_t prefix_len) {
        sockaddr_in6 baddr(m_addr);
        baddr.sin6_addr.s6_addr[prefix_len / 8] |= CreateMask<uint8_t>(prefix_len % 8);
        for (uint32_t i = 0; i < prefix_len / 8 + 1; ++i) {
            baddr.sin6_addr.s6_addr[i] = 0xff;
        }
        return IPv6Address::ptr(new IPv6Address(baddr));
    }
    
    IPAddress::ptr IPv6Address::networkAddress(uint32_t prefix_len) {
        struct sockaddr_in6 baddr(m_addr);
        baddr.sin6_addr.s6_addr[prefix_len / 8] &= CreateMask<uint8_t>(prefix_len % 8);
        for (int i = prefix_len / 8 + 1; i < 16; ++i) {
            baddr.sin6_addr.s6_addr[i] = 0x00;
        }
        return IPv6Address::ptr(new IPv6Address(baddr));
    
    }
    
    IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
        struct sockaddr_in6 baddr;
        memset(&baddr, 0, sizeof(baddr));
        baddr.sin6_family = AF_INET6;
        baddr.sin6_addr.s6_addr[prefix_len / 8] = ~CreateMask<uint8_t>(prefix_len % 8);

        for (uint32_t i = 0; i < prefix_len / 8; ++i) {
            baddr.sin6_addr.s6_addr[i] = 0xff;
        }

        return IPv6Address::ptr(new IPv6Address(baddr));
    }

    uint32_t IPv6Address::getPort() const {
        return byteswapOnLittleEndian(m_addr.sin6_port);
    }

    void IPv6Address::setPort(uint16_t port) {
        m_addr.sin6_port = byteswapOnLittleEndian(port);
    }

    static const size_t MAX_PATH_LEN = sizeof(((sockaddr_un*)0)->sun_path) - 1;
    UnixAddress::UnixAddress() {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sun_family = AF_UNIX;
        m_length = MAX_PATH_LEN + offsetof(sockaddr_un, sun_path);
    }

    UnixAddress::UnixAddress(const std::string& path) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sun_family = AF_UNIX;
        m_length = path.size() + 1;
        if (!path.empty() && path[0] == '\0') {
            --m_length;
        }
        if (m_length >sizeof(m_addr.sun_path)) {
            throw std::logic_error("path too long!");
        }
        memcpy(m_addr.sun_path, path.c_str(), m_length);
        m_length += offsetof(sockaddr_un, sun_path);
    }

    void UnixAddress::setAddrLen(uint32_t v) {
        m_length = v;
    }

    sockaddr* UnixAddress::getAddr() {
        return (sockaddr*)&m_addr;
    }
    
    const sockaddr* UnixAddress::getAddr() const {
        return (sockaddr*)&m_addr;
    }

    socklen_t UnixAddress::getAddrLen() const {
        return m_length;
    }

    std::string UnixAddress::getPath() const {
        std::stringstream ss;
        if (m_length > offsetof(sockaddr_un, sun_path) 
                && m_addr.sun_path[0] == '\0') {
            ss << "\\0" << std::string(m_addr.sun_path + 1, 
                    m_length - offsetof(sockaddr_un, sun_path) - 1);
        } else {
            ss << m_addr.sun_path;
        }
        return ss.str();
    }

    std::ostream& UnixAddress::insert(std::ostream& os) const {
        if(m_length > offsetof(sockaddr_un, sun_path) 
                && m_addr.sun_path[0] == '\0') {
            return os << "\\0" << std::string(m_addr.sun_path + 1,
                    m_length - offsetof(sockaddr_un, sun_path) - 1);
        }
        return os << m_addr.sun_path;
    }

    UnknowAddress::UnknowAddress(int family) {
        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sa_family = family;
    }

    UnknowAddress::UnknowAddress(const sockaddr& addr) {
        m_addr = addr;
    }

    sockaddr* UnknowAddress::getAddr() {
        return &m_addr;
    }
    
    const sockaddr* UnknowAddress::getAddr() const {
        return (sockaddr*)&m_addr;
    }

    socklen_t UnknowAddress::getAddrLen() const {
        return sizeof(m_addr);
    }

    std::ostream& UnknowAddress::insert(std::ostream& os) const  {
        os << "[UnknowAddress family=" <<m_addr.sa_family << "]";
        return os;
    }

}