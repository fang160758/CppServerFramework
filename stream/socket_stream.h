#pragma once

#include "../inc/stream.h"
#include "../inc/socket.h"
#include "../inc/address.h"
#include <memory>
#include <string>


namespace fang {
class SocketStream : public Stream {
public:
    typedef std::shared_ptr<SocketStream> ptr;

    SocketStream(Socket::ptr sock, bool owner = true);
    ~SocketStream();

    virtual int read(void* buffer, size_t length) override;
    virtual int read(ByteArray::ptr ba, size_t length) override;

    virtual int write(const void* buffer, size_t length) override;
    virtual int write(ByteArray::ptr, size_t length) override;

    virtual void close() override;

    Socket::ptr getSocket() const { return m_socket; }
    bool isConnect() const;

    Address::ptr getRemoteAddress();
    Address::ptr getLocalAddress();
    std::string getRemoteAddressString();
    std::string getLocalAddressString();

protected:
    Socket::ptr m_socket;
    bool m_owner;
};
}
