#include "Acceptor.h"

Acceptor::Acceptor(EventLoop* loop, const sockaddr_in& listenAddr)
    : listening_(false)
    , listenfd_(-1)
    , listenAddr_(listenAddr)
    , loop_(loop)
    , acceptChannel_(loop)
{
    listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd_, (sockaddr*)&listenAddr_, sizeof(listenAddr_));
    acceptChannel_.SetReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::Listen()
{
    assert(!listening_);
    {
        listening_ = true;
        listen(listenfd_, 20);
        acceptChannel_.EnableReading();
    }
}

void Acceptor::handleRead()
{
    sockaddr_in peerAddr;
    socklen_t addrLen;
    // FIXME loop until no more
    int connfd = accept(listenfd_, (sockaddr*)&listenAddr_, &addrLen);
    if (connfd >= 0) {
        // string hostport = peerAddr.toIpPort();
        // LOG_TRACE << "Accepts of " << hostport;
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        } else {
            close(connfd);
        }
    } else {
        return;
    }
}