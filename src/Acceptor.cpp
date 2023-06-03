#include "Acceptor.h"
#include "LinuxHead.h"
#include "util.hpp"
#include <cstring>
#include <glog/logging.h>


Acceptor::Acceptor(EventLoop* loop, const sockaddr_in& listenAddr)
    : listening_(false)
    , listenfd_(socket(AF_INET, SOCK_STREAM, 0))
    , listenAddr_(listenAddr)
    , loop_(loop)
    , acceptChannel_(listenfd_,loop)
{
    // listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    int e = bind(listenfd_, (sockaddr*)&listenAddr_, sizeof(listenAddr_));
    if (e < 0)
        LOG(FATAL) << "Failt to bind to listen: "<<strerror(-e)<<" \n";
    acceptChannel_.SetReadCallback(std::bind(&Acceptor::handleRead, this, std::placeholders::_1));
}

void Acceptor::Listen()
{
    assert(!listening_);
    {
        listening_ = true;
        int e = listen(listenfd_, 20);
        if (e < 0)
            LOG(FATAL) << "Failt to bind to listen: " << strerror(e) << " \n";
        acceptChannel_.EnableAccepting();
    }
}

void Acceptor::handleRead(TimeStamp timeStamp)
{
    sockaddr_in peerAddr;
    socklen_t addrLen;
    int connfd;
    // FIXME loop until no more
    if ((connfd = acceptChannel_.GetN()) != -1) {
        getpeername(connfd, (sockaddr*)&peerAddr, &addrLen);
    } else {
        connfd = accept(listenfd_, (sockaddr*)&peerAddr, &addrLen);
    }
    if (connfd >= 0) {
        LOG(INFO) << "Accept connection from " << convertAddr(peerAddr) <<" at "<<timeStamp<< "\n";
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddr);
        } else {
            close(connfd);
        }
    } else {
        LOG(ERROR) << "Fail to accept connection\n";
    }
}