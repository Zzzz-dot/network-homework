#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Acceptor.h"
#include "TCPConn.h"
#include "LinuxHead.h"
#include <map>

class TCPServer {

public:
    TCPServer(const sockaddr_in& listenAddr, const std::string& name, int numThread = 0);
    void Start();

    /// Set message callback.
    /// Not thread safe.
    void SetMessageCallback(const ReadEventCallback& cb)
    {
        messageCallback_ = cb;
    }

    /// Set write complete callback.
    /// Not thread safe.
    void SetWriteCompleteCallback(const EventCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }

private:
    using ConnectionMap = std::map<std::string, TCPConnPtr>;
    bool started_;
    std::string name_;
    int numThreads_;

    int nextConnId_;
    ConnectionMap connections_;
    Acceptor acceptor_;
    EventLoop baseLoop_;
    EventLoopThreadPool threadPool_;

    EventCallback connectionCallback_;
    ReadEventCallback messageCallback_;
    EventCallback writeCompleteCallback_;

private:
    void newConnection(int connfd, const sockaddr_in& peerAddr);
    void removeConnection(const TCPConnPtr& conn, TimeStamp timestamp);
};

#endif