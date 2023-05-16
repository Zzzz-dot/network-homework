#include "TCPServer.h"
#include "TCPConn.h"
#include "util.hpp"

void TCPServer::Start()
{
    if (!started_) {
        started_ = true;
        threadPool_.Start();
        acceptor_.Listen();
        baseLoop_.Loop();
    }
}

TCPServer::TCPServer(const sockaddr_in& listenAddr, const std::string& name, int numThread)
    : started_(false)
    , name_(name)
    , numThreads_(numThread)
    , acceptor_(&baseLoop_, listenAddr)
    , threadPool_(&baseLoop_, numThreads_, name_)

{
    acceptor_.SetNewConnectionCallback(std::bind(&TCPServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));
}

void TCPServer::newConnection(int connfd, const sockaddr_in& peerAddr)
{
    EventLoop* loop = threadPool_.GetNextLoop();
    std::string connName=name_+convertAddr(peerAddr)+"#"+std::to_string(nextConnId_);

    printf("TcpServer::newConnection [%s]\n",connName.c_str());

    sockaddr_in localAddr;
    socklen_t len;
    getsockname(connfd, (sockaddr*)&localAddr, &len);


    TCPConnPtr conn=new TCPConn(loop,connfd,connName,localAddr,peerAddr);
    connections_[connName] = conn;
    conn->SetMessageCallback(messageCallback_);
    conn->SetWriteCompleteCallback(writeCompleteCallback_);
    conn->SetCloseCallback(std::bind(&TCPServer::removeConnection, this, std::placeholders::_1, std::placeholders::_2));
    loop->NewTask(std::bind(&TCPConn::Init, conn));
}

void TCPServer::removeConnection(const TCPConnPtr& conn, TimeStamp timestamp)
{
    auto task = [this, conn]() -> void {
        this->connections_.erase(conn->ConnName());
        delete conn;
    };
    baseLoop_.NewTask(task);
}
