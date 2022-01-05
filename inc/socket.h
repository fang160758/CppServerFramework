/**
 * @file socket.h
 * @Synopsis  socket封装
 * @author Fang
 * @version 1.0
 * @date 2021-12-30
 */

#ifndef __FANG_SOCKET_H__
#define __FANG_SOCKET_H__

#include <memory>
#include <string>
#include <netinet/tcp.h>
#include <sys/types.h>
#include "address.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace fang {
/**
* @Synopsis  Socket封装
*/
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

    /**
     * @Synopsis  创建socket
     *
     * @Returns   返回socket智能指针
     */
    static Socket::ptr CreateTCPScoket();               
    static Socket::ptr CreateUDPScoket();
    static Socket::ptr CreateTCPScoket6();
    static Socket::ptr CreateUDPScoket6();
    static Socket::ptr CreateTCP(Address::ptr addr);
    static Socket::ptr CreateUDP(Address::ptr addr);
    static Socket::ptr CreateUnixTCPScoket();
    static Socket::ptr CreateUnixUDPScoket();

    /**
     * @Synopsis  构造函数
     *
     * @Param[in] family 协议族
     * @Param[in] type socket类型
     * @Param[in] protocol 协议
     */
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
    bool setOption(int level, int optname, const void* result, socklen_t len);

    template<typename T>
    bool getOption(int level, int optname, T& result) {
        socklen_t length = sizeof(T);
        return getOption(level, optname, &result, &length);
    }

    template<typename T>
    bool setOption(int level, int option, const T& result) {
        return setOption(level, option, &result, sizeof(T));
    }


    /**
     * @Synopsis  接收connect 连接
     *
     * @Returns   成功返回新连接的socket，失败返回nullptr
     */
    virtual Socket::ptr accept();

    /**
     * @Synopsis  绑定地址
     *
     * @Param[in] Address::ptr 地址
     */
    virtual bool bind(const Address::ptr address);

    /**
     * @Synopsis  发起连接
     *
     * @Param[in] Address::ptr 地址
     * @Param[in] timeout_ms 超时时间(毫秒)
     */
    virtual bool connect(const Address::ptr address, uint64_t timeout_ms = -1); // 连接地址
    virtual bool reconnect(uint64_t timeout_ms = -1);

    /**
     * @Synopsis  监听socket
     *
     * @Param[in] backlog 最大监听数量
     */
    virtual bool listen(int backlog = SOMAXCONN);

    /**
     * @Synopsis  关闭socket
     */
    virtual bool close();

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

    /**
     * @Synopsis  获本地端地址
     */
    Address::ptr getLocalAddress(); 
    
    /**
     * @Synopsis  获取远端地址
     */
    Address::ptr getRemoteAddress();
    bool isConnected() const { return m_isConnected; }


protected:

    /**
     * @Synopsis  初始化socket
     */
    void initSock();

    /**
     * @Synopsis  创建socket
     */
    void newSock();

    /**
     * @Synopsis  初始化sock
     */
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

class SSLSocket : public Socket {
public:
    typedef std::shared_ptr<SSLSocket> ptr;

    static SSLSocket::ptr CreateTCP(fang::Address::ptr address);
    static SSLSocket::ptr CreateTCPScoket();
    static SSLSocket::ptr CreateTCPScoket6();

    SSLSocket(int family, int type, int protocol = 0);

    virtual Socket::ptr accept() override;
    virtual bool bind(const Address::ptr) override;
    virtual bool connect(const Address::ptr, uint64_t timeout_ms = -1) override;
    virtual bool listen(int backlog = SOMAXCONN) override;
    virtual bool close() override;
                
    virtual int send(const void* buffer, size_t length, int flags = 0) override;
    virtual int send(const struct iovec* buffers, size_t length, int flags = 0) override;
    virtual int sendTo(const void* buffer, size_t length, Address::ptr, int flags = 0) override;
    virtual int sendTo(const struct iovec* buffers, size_t length, Address::ptr, int flags = 0) override;

    virtual int recv(void* buffer, size_t length, int flags = 0) override;
    virtual int recv(struct iovec* buffers, size_t length, int flags = 0) override;
    virtual int recvFrom(void* buffer, size_t length, Address::ptr, int flags = 0) override;
    virtual int recvFrom(struct iovec* buffers, size_t length, Address::ptr, int flags = 0) override;

    bool loadCertificates(const std::string& cerf_file, const std::string& key_file);

    virtual std::ostream& dump(std::ostream& os) const override;

protected:
    virtual bool init(int sock) override;

private:
    std::shared_ptr<SSL_CTX> m_ctx;
    std::shared_ptr<SSL> m_ssl;
};

std::ostream& operator<<(std::ostream& os, const Socket& sock);

}


#endif
