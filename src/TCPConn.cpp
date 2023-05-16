#include "TCPConn.h"

TCPConn::TCPConn(EventLoop* loop, int connfd, const std::string connName, const sockaddr_in& localAddr, const sockaddr_in& remoteAddr)
    : state_(kConnecting)
    , loop_(loop)
    , connName_(connName)
    , localAddr_(localAddr)
    , remoteAddr_(remoteAddr)
    , channel_(loop)
{
    channel_.SetReadCallback(
        std::bind(&TCPConn::handleRead, this, std::placeholders::_1));
    channel_.SetWriteCallback(
        std::bind(&TCPConn::handleWrite, this, std::placeholders::_1));
    channel_.SetCloseCallback(
        std::bind(&TCPConn::handleClose, this, std::placeholders::_1));
    channel_.SetErrorCallback(
        std::bind(&TCPConn::handleError, this, std::placeholders::_1));
}

TCPConn::~TCPConn()
{
    close(connfd_);
}

void TCPConn::Init()
{
    assert(state_ == kConnecting);
    state_ = kConnected;
    channel_.EnableReading();
}

void TCPConn::Send(const void* data, size_t len)
{
    if (state_ == kConnected) {
        write(connfd_, data, len);
    }
}

void TCPConn::handleRead(TimeStamp timeStamp)
{
    char buf[BUFSIZ];
    int n = read(connfd_, buf, BUFSIZ);
    if (n > 0) {
        messageCallback_(this, buf, n,timeStamp);
    } else if (n == 0) {
        handleClose(timeStamp);
    } else {
        handleError(timeStamp);
    }
}

void TCPConn::handleWrite(TimeStamp timeStamp)
{
    printf("%d: handleWrite\n", timeStamp);
}

void TCPConn::handleClose(TimeStamp timeStamp)
{
    state_ = kDisconnected;
    channel_.DisableAll();
    closeCallback_(this,timeStamp);
}

void TCPConn::handleError(TimeStamp timeStamp)
{
    printf("%d: %s\n", timeStamp, strerror(errno));
}