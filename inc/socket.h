#ifndef __FANG_SOCKET_H__
#define __FANG_SOCKET_H__

#include <memory>
#include <string>
#include <sys/types.h>
#include "address.h"


namespace fang {
class Socket : public std::enable_shared_from_this<Socket> {
public:
    typedef std::shared_ptr<Socket> ptr;
    typedef std::weak_ptr<Socket> weak_ptr;

    enum type {
        TCP = SOCK_STREAM, // TCP类型
        UDP = SOCK_DGRAM   // UDP类型
    };

    enum family {
        IPv4 = AF_INET,    // IPv4
        IPv6 = AF_INET6,   // IPv6
        UNIX = AF_UNIX     // Unix
    };

    static Socket::ptr CreateTCPScoket();               
    static Socket::ptr CreateUDPScoket();
    static Socket::ptr CreateTCPScoket6();
    static Socket::ptr CreateUDPScoket6();
    static Socket::ptr CreateTCP(Address::ptr addr);
    static Socket::ptr CreateUDP(Address::ptr addr);
    static Socket::ptr CreateUnixTCPScoket();
    static Socket::ptr CreateUnixUDPScoket();

    Socket(int family, int type, int protocol = 0); // 构造器
    ~Socket();

    //设置/获取sock发送超时时间。
    uint64_t getSendTimeout();
    void setSendTimeout(uint64_t v);

    //设置/获取sock接收超时时间
    uint64_t getRecvTimeout();
    void setRecvTimeout(uint64_t v);

    //设置sock选项，是对set/getsockopt的封装。
    bool getOption(int level, int optname, void* result, socklen_t* len);
    bool setOption(int level, int optname, const void* result, socklen_t len);

    template<typename T>
    bool getOption(int level, int optname, T& result) {
        socklen_t length = sizeof(T);
        return getOption(level, optname, &result, &length);
    }

    template<typename T>
    bool setOption(int level, int option, const T& result) {
        socklen_t length = sizeof(T);
        return setOption(level, option, &result, &length);
    }

    virtual Socket::ptr accept(); //接收connect的连接
    virtual bool bind(const Address::ptr);
    virtual bool connect(const Address::ptr, uint64_t timeout_ms = -1); // 连接地址
    virtual bool reconnect(uint64_t timeout_ms = -1);
    virtual bool listen(int backlog = SOMAXCONN); // 监听socket
    virtual bool close(); // 关闭socket
                
    virtual int send(const void* buffer, size_t length, int flags = 0);
    virtual int send(const struct iovec* buffers, size_t length, int flags = 0);
    virtual int sendTo(const void* buffer, size_t length, Address::ptr, int flags = 0);
    virtual int sendTo(const struct iovec* buffers, size_t length, Address::ptr, int flags = 0);

    virtual int recv(void* buffer, size_t length, int flags = 0);
    virtual int recv(struct iovec* buffers, size_t length, int flags = 0);
    virtual int recvFrom(void* buffer, size_t length, Address::ptr, int flags = 0);
    virtual int recvFrom(struct iovec* buffers, size_t length, Address::ptr, int flags = 0);
    
    bool cancelRead();
    bool cancelWirte();
    bool cancelAll();
    bool cancelAccept();

    bool isValid() const;
    int getError();

    virtual std::ostream& dump(std::ostream& os) const;
    virtual std::string toString() const;
    int getFamily() const { return m_family; }
    int getType() const { return m_type; }
    int getProtocol() const { return m_protocol; }
    int getSocket() const { return m_sock; }
    Address::ptr getLocalAddress(); 
    Address::ptr getRemoteAddress();
    bool isConnected() const { return m_isConnected; }


protected:
    void initSock();
    void newSock();
    virtual bool init(int sock);


protected:
    int m_sock;
    int m_family;
    int m_type;
    int m_protocol;
    bool m_isConnected;

    Address::ptr m_localAddress;
    Address::ptr m_remoteAddress;
};

//class SSLSocket : public Socket {
//public:
//    typedef std::shared_ptr<SSLSocket> ptr;
//
//    static SSLSocket::ptr CreateTCP(fang::Address::ptr address);
//    static SSLSocket::ptr CreateTCPScoket();
//    static SSLSocket::ptr CreateTCPScoket6();
//
//    SSLSocket(int family, int type, int protocol = 0);
//
//    virtual Socket::ptr accept() override;
//    virtual bool bind(const Address::ptr) override;
//    virtual bool connect(const Address::ptr, uint64_t timeout_ms = -1) override;
//    virtual bool reconnect(uint64_t timeout_ms = -1) override;
//    virtual bool listen(int backlog = SOMAXCONN) override;
//    virtual bool close() override;
//                
//    virtual int send(const void* buffer, size_t length, int flags = 0) override;
//    virtual int send(const struct iovec* buffers, size_t length, int flags = 0) override;
//    virtual int sendTo(const void* buffer, size_t length, Address::ptr, int flags = 0) override;
//    virtual int sendTo(const struct iovec* buffers, size_t length, Address::ptr, int flags = 0) override;
//
//    virtual int recv(void* buffer, size_t length, int flags = 0) override;
//    virtual int recv(struct iovec* buffers, size_t length, int flags = 0) override;
//    virtual int recvFrom(void* buffer, size_t length, Address::ptr, int flags = 0) override;
//    virtual int recvFrom(struct iovec* buffers, size_t length, Address::ptr, int flags = 0) override;
//
//    bool loadCertificates(const std::string& cerf_file, const std::string& key_file);
//
//    virtual std::ostream& dump(std::ostream& os) const override;
//
//protected:
//    virtual bool init(int sock) override;
//
//private:
//    std::shared_ptr<SSL_CTX> m_ctx;
//    std::shared_ptr<SSL> m_ssl;
//};

std::ostream& operator<<(std::ostream& os, const Socket& sock);
}






#endif
