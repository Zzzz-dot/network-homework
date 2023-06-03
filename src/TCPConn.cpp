#include "TCPConn.h"
#include <cerrno>
#include <cstring>
#include <glog/logging.h>

TCPConn::TCPConn(EventLoop* loop, int connfd, const std::string connName, const sockaddr_in& localAddr, const sockaddr_in& remoteAddr)
    : state_(kConnecting)
    , connfd_(connfd)
    , connName_(connName)
    , localAddr_(localAddr)
    , remoteAddr_(remoteAddr)
    , loop_(loop)
    , channel_(connfd, loop)
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
    state_ = kDisconnected;
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
    assert(state_ == kConnected);
    write(connfd_, data, len);
}

void TCPConn::handleRead(TimeStamp timeStamp)
{
    assert(state_ == kConnected);
    char readbuf[BUFSIZ];
    int n;
    char *buf;
    if ((n = channel_.GetN()) != -1) {
        buf=(char *)channel_.ReadBuf();
    } else {
        buf=readbuf;
        n = read(connfd_, buf, BUFSIZ);
    }
    if (n > 0) {
        messageCallback_(this, buf, n, timeStamp);
    } else if (n == 0) {
        handleClose(timeStamp);
    } else {
        handleError(timeStamp);
    }
}

void TCPConn::handleWrite(TimeStamp timeStamp)
{
    LOG(INFO) << "Write finish at " << timeStamp << "\n";
}

void TCPConn::handleClose(TimeStamp timeStamp)
{
    LOG(INFO) << "Connection close at " << timeStamp << "\n";
    state_ = kDisconnecting;
    channel_.Remove();
    closeCallback_(this, timeStamp);
}

void TCPConn::handleError(TimeStamp timeStamp)
{
    LOG(ERROR) << "Error: " << strerror(errno) <<" at " << timeStamp<<"\n";
}