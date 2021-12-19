#ifndef __FANG_SOCKET_H__
#define __FANG_SOCKET_H__

#include <memory>
#include <sys/types.h>
#include "address.h"


namespace fang {
    class Socket : public std::enable_shared_from_this<Socket> {
        public:
            typedef std::shared_ptr<Socket> ptr;
            typedef std::weak_ptr<Socket> weak_ptr;

            enum type {
                TCP = SOCK_STREAM,
                UDP = SOCK_DGRAM
            };

            enum family {
                IPv4 = AF_INET,
                IPv6 = AF_INET6,
                UNIX = AF_UNIX
            };

            static Socket::ptr CreateTCPScoket();
            static Socket::ptr CreateUDPScoket();
            static Socket::ptr CreateTCPScoket6();
            static Socket::ptr CreateUDPScoket6();
            static Socket::ptr CreateTCP(Address::ptr addr);
            static Socket::ptr CreateUDP(Address::ptr addr);
            static Socket::ptr CreateUnixTCPScoket();
            static Socket::ptr CreateUnixUDPScoket();

            Socket(int family, int type, int protocol = 0);
            ~Socket();

            //设置/获取sock发送超时时间。
            uint64_t getSendTimeout();
            void setSendTimeout(uint64_t v);

            //设置/获取sock接收超时时间
            uint64_t getRecvTimeout();
            void setRecvTimeout(uint64_t v);

            //设置sock选项，是对set/getsockopt的封装。
            bool getOption(int level, int optname, void* result, socklen_t* len);
            bool setOption(int level, int optname, const void* result, socklen_t len = 0);

            Socket::ptr accept();

            bool init(int sock);
            bool bind(const Address::ptr);
            bool connect(const Address::ptr, uint64_t timeout_ms = -1);
            bool listen(int backlog = SOMAXCONN);
            bool close();
            
            int send(const void* buffer, size_t length, int flags = 0);
            int send(const struct iovec* buffers, size_t length, int flags = 0);
            int sendTo(const void* buffer, size_t length, Address::ptr, int flags = 0);
            int sendTo(const struct iovec* buffers, size_t length, Address::ptr, int flags = 0);

            int recv(void* buffer, size_t length, int flags = 0);
            int recv(struct iovec* buffers, size_t length, int flags = 0);
            int recvFrom(void* buffer, size_t length, Address::ptr, int flags = 0);
            int recvFrom(struct iovec* buffers, size_t length, Address::ptr, int flags = 0);
            
            int getError();
            bool cancelRead();
            bool cancelWirte();
            bool cancelAll();

            bool isValid() const;

            std::ostream& dump(std::ostream& os) const;

            int getFamily() const { return m_family; }
            int getType() const { return m_type; }
            int getProtocol() const { return m_protocol; }
            Address::ptr getLocalAddress(); 
            Address::ptr getRemoteAddress();
        

        private:
            void initSock();
            void newSock();

        private:
            int m_sock;
            int m_family;
            int m_type;
            int m_protocol;
            bool m_isConnected;

            Address::ptr m_localAddress;
            Address::ptr m_remoteAddress;
    };
}






#endif
