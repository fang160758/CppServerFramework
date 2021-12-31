/**
 * @file address.h
 * @Synopsis  网络地址的封装
 * @author Fang
 * @version 1.0
 * @date 2021-12-31
 */
#ifndef __FANG_ADDRESS_H__
#define __FANG_ADDRESS_H__

#include <memory>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <map>

namespace fang {
    
class IPAddress;

/**
 * @Synopsis  网络地址基类
 */
class Address {
public:
   typedef std::shared_ptr<Address> ptr; 

   /**
    * @Synopsis  通过sockaddr 创建Address
    *
    * @Param[in] addr sockaddr 指针
    * @Param[in] addrlen 长度
    *
    * @Returns   Address
    */
   static Address::ptr Create(const sockaddr* addr, socklen_t addrlen);
   
   /**
    * @Synopsis  通过host地址返回所有满足条件的Address
    *
    * @Param[in] result 满足条件的Address集合
    * @Param[in] host 域名/主机名/ip地址[www.baidu.com:80] [11.11.11.11:80]
    * @Param[in] family 协议族
    * @Param[in] type socket类型
    * @Param[in] protocol 协议
    *
    * @Returns   是否成功
    */
   static bool Lookup(std::vector<Address::ptr>& result, const std::string& host,
           int family = AF_INET, int type = 0, int protocol = 0);

   /**
    * @Synopsis  返回任意一个对应的Address 实际是调用上面的函数
    *
    * @Returns   对应的Address
    */
   static Address::ptr LookupAny(const std::string& host, 
           int family = AF_INET, int type = 0, int protocol = 0);

   
   /**
    * @Synopsis  返回任意一个满足条件的IpAddress
    *
    * @Returns   对应的IPAddress智能指针
    */
   static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,
           int family = AF_INET, int type = 0, int protocol = 0);
   
   /**
    * @Synopsis  获取主机的网卡信息<网卡名，地址，子网掩码位数>
    *
    * @Param[in] result 保存所有的返回主机地址
    * @Param[in] family 协议族(默认无关)
    */
   static bool GetInterfaceAddresses(std::multimap<std::string,
           std::pair<Address::ptr, uint32_t> >& result, int family = AF_UNSPEC);

   /**
    * @Synopsis 获取指定网卡的地址和子网掩码位数 
    *
    * @Param[in] iface 网卡名称
    *
    * @Returns   
    */
   static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t> >& result, 
           const std::string& iface, int family = AF_INET);
   

   virtual ~Address() {};

   int getFamily() const;

   virtual sockaddr* getAddr() = 0;
   virtual const sockaddr* getAddr() const = 0;
   virtual socklen_t getAddrLen() const = 0;

   /**
    * @Synopsis  可读性输出地址
    */
   virtual std::ostream& insert(std::ostream& os) const = 0;
   
   /**
    * @Synopsis  返回可读性字符串
    */
   std::string toString();

   bool operator<(const Address& rhs) const;
   bool operator==(const Address& rhs) const;
   bool operator!=(const Address& rhs) const;

};

/**
 * @Synopsis  IP地址基类
 */
class IPAddress: public Address {
public:
    typedef std::shared_ptr<IPAddress> ptr;

    /**
     * @Synopsis  根据host创建一个IPAddress
     *
     * @Param[in] address host
     * @Param[in] port 端口
     */
    static IPAddress::ptr Create(const char* address, uint16_t port = 0);

    /**
     * @Synopsis  获取广播地址
     *
     * @Param[in] prefix_len 掩码长度
     */
    virtual IPAddress::ptr boradcastAddress(uint32_t prefix_len) = 0;
    
    /**
     * @Synopsis  获取网络地址
     *
     * @Param[in] prefix_len 掩码长度
     */
    virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;
    
    /**
     * @Synopsis  获取子网掩码地址
     * 
     * @Param[in] prefix_len 掩码长度
     */
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

    virtual uint32_t getPort() const = 0;
    virtual void setPort(uint16_t port) = 0;
};

class IPv4Address: public IPAddress {
    public:
        typedef std::shared_ptr<IPv4Address> ptr;

        static IPv4Address::ptr Create(const char* address, uint16_t port);

        IPv4Address(const struct sockaddr_in& address);
        IPv4Address(uint32_t address = INADDR_ANY, uint16_t port = 0);
        
        sockaddr* getAddr() override;
        const sockaddr* getAddr() const override;
        socklen_t getAddrLen() const override;

        std::ostream& insert(std::ostream& os) const override;
        IPAddress::ptr boradcastAddress(uint32_t prefix_len) override; 
        IPAddress::ptr networkAddress(uint32_t prefix_len) override;
        IPAddress::ptr subnetMask(uint32_t prefix_len) override;
        
        uint32_t getPort() const override;
        void setPort(uint16_t port) override;
    private:
        struct sockaddr_in m_addr;
};

class IPv6Address: public IPAddress {
    public:
        typedef std::shared_ptr<IPv6Address> ptr;
        IPv6Address();
        static IPv6Address::ptr Create(const char* address, uint16_t port); 
        IPv6Address(const struct sockaddr_in6& address);
        IPv6Address(const uint8_t address[16], uint16_t port = 0);
        
        sockaddr* getAddr() override;
        const sockaddr* getAddr() const override;
        socklen_t getAddrLen() const override;

        std::ostream& insert(std::ostream& os) const override;
        IPAddress::ptr boradcastAddress(uint32_t prefix_len) override; 
        IPAddress::ptr networkAddress(uint32_t prefix_len) override;
        IPAddress::ptr subnetMask(uint32_t prefix_len) override;

        uint32_t getPort() const override;
        void setPort(uint16_t port) override;
    private:
        struct sockaddr_in6 m_addr;
};   

class UnixAddress: public Address {
    public:
        typedef std::shared_ptr<UnixAddress> ptr;
        UnixAddress();
        UnixAddress(const std::string& path);
        
        sockaddr* getAddr() override;
        const sockaddr* getAddr() const override;
        socklen_t getAddrLen() const override;

        void setAddrLen(uint32_t v);
        std::string getPath() const;
        std::ostream& insert(std::ostream& os) const override;
        
    private:
        struct sockaddr_un m_addr;
        socklen_t m_length;
        
};

class UnknowAddress :public Address {
    public:
        typedef std::shared_ptr<UnixAddress> ptr;

        UnknowAddress(int family);
        UnknowAddress(const sockaddr& addr);
        
        sockaddr* getAddr() override;
        const sockaddr* getAddr() const override;

        socklen_t getAddrLen() const override;

        std::ostream& insert(std::ostream& os) const override;

    private:
        struct sockaddr m_addr;
};
std::ostream& operator<<(std::ostream& os, const Address& addr);
}
#endif





