#ifndef _TCPCONN_H
#define _TCPCONN_H
#include "Channel.h"
#include "LinuxHead.h"

class EventLoop;

class TCPConn {
public:
    TCPConn(EventLoop* loop, int connfd, const std::string connName,const sockaddr_in& localAddr, const sockaddr_in& remoteAddr);
    
    ~TCPConn();

    void Init();

    void SetMessageCallback(const ReadEventCallback& cb)
    {
        messageCallback_ = cb;
    }
    void SetWriteCompleteCallback(const EventCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }
    void SetCloseCallback(const EventCallback& cb)
    {
        closeCallback_ = cb;
    }

    const std::string& ConnName()
    {
        return  connName_;
    }

    void Send(const void* data, size_t len);

private:
    enum StateE { kDisconnected,
        kConnecting,
        kConnected,
        kDisconnecting };
    StateE state_;
    int connfd_;
    std::string connName_;
    sockaddr_in localAddr_;
    sockaddr_in remoteAddr_;

private:
    EventLoop* loop_;
    TCPChannel channel_;
    ReadEventCallback messageCallback_;
    EventCallback writeCompleteCallback_;
    EventCallback closeCallback_;

    void handleRead(TimeStamp timeStamp);
    void handleWrite(TimeStamp timeStamp);
    void handleClose(TimeStamp timeStamp);
    void handleError(TimeStamp timeStamp);
};

#endif