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

    class Address {
        public:
           typedef std::shared_ptr<Address> ptr; 

           static Address::ptr Create(const sockaddr* addr, socklen_t addrlen); //通过sockaddr指针创建address
           
           static bool Lookup(std::vector<Address::ptr>& result, const std::string& host,
                   int family = AF_INET, int type = 0, int protocol = 0);
           static Address::ptr LookupAny(const std::string& host, 
                   int family = AF_INET, int type = 0, int protocol = 0);
           static std::shared_ptr<IPAddress> LookupAnyIPAddress(const std::string& host,
                   int family = AF_INET, int type = 0, int protocol = 0);
           
           
           static bool GetInterfaceAddresses(std::multimap<std::string,
                   std::pair<Address::ptr, uint32_t> >& result, int family = AF_UNSPEC);
           static bool GetInterfaceAddresses(std::vector<std::pair<Address::ptr, uint32_t> >& result, 
                   const std::string& iface, int family = AF_UNSPEC);
           

           virtual ~Address() {};

           int getFamily() const;

           virtual sockaddr* getAddr() = 0;
           virtual const sockaddr* getAddr() const = 0;
           virtual socklen_t getAddrLen() const = 0;

           virtual std::ostream& insert(std::ostream& os) const = 0;
           std::string toString();

           bool operator<(const Address& rhs) const;
           bool operator==(const Address& rhs) const;
           bool operator!=(const Address& rhs) const;

    };

    class IPAddress: public Address {
        public:
            typedef std::shared_ptr<IPAddress> ptr;

            virtual IPAddress::ptr boradcastAddress(uint32_t prefix_len) = 0;
            virtual IPAddress::ptr networkAddress(uint32_t prefix_len) = 0;
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
}
#endif





