#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H
#include "Channel.h"
#include "LinuxHead.h"
class EventLoop;

class Acceptor {
public:
    using NewConnectionCallback = std::function<void(int, const sockaddr_in&)>;
    Acceptor(EventLoop* loop, const sockaddr_in& listenAddr);
    void SetNewConnectionCallback(const NewConnectionCallback& cb)
    {
        newConnectionCallback_ = cb;
    }
    void Listen();

private:
    bool listening_;
    int listenfd_;
    sockaddr_in listenAddr_;
    EventLoop* loop_;
    TCPChannel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;

private:
    void handleRead(TimeStamp timeStamp);
};

#endif