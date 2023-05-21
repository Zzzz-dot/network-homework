#include "Acceptor.h"
#include "LinuxHead.h"
#include "util.hpp"
#include <glog/logging.h>


Acceptor::Acceptor(EventLoop* loop, const sockaddr_in& listenAddr)
    : listening_(false)
    , listenfd_(socket(AF_INET, SOCK_STREAM, 0))
    , listenAddr_(listenAddr)
    , loop_(loop)
    , acceptChannel_(listenfd_,loop)
{
    // listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
    bind(listenfd_, (sockaddr*)&listenAddr_, sizeof(listenAddr_));
    acceptChannel_.SetReadCallback(std::bind(&Acceptor::handleRead, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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

void Acceptor::handleRead(TimeStamp timeStamp,int bid=-1, void* buf=nullptr)
{
    sockaddr_in peerAddr;
    socklen_t addrLen;
    // FIXME loop until no more
    int connfd = accept(listenfd_, (sockaddr*)&peerAddr, &addrLen);
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