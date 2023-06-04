#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Acceptor.h"
#include "TCPConn.h"
#include "LinuxHead.h"
#include <map>
#include <netinet/in.h>

class FILEOperator {

public:
    FILEOperator(const std::string& name, int numThread = 1);
    void Start();

    /// Set message callback.
    /// Not thread safe.
    void SetReadCompleteCallback(const FILEReadCallback& cb)
    {
        readCompleteCallback_ = cb;
    }

    /// Set write complete callback.
    /// Not thread safe.
    void SetWriteCompleteCallback(const FILEWriteCallback& cb)
    {
        writeCompleteCallback_ = cb;
    }

    // 读取请求，读取的内容存在 channel，随后在回调函数中处理
    void Read(int fd);

    // 写入请求，附带写入的内容及长度
    void Write(int fd, const char* buf, int len);

private:
    using FILEChannelPtr=FILEChannel*;
    using FILEChannelMap = std::map<std::string, FILEChannelPtr>;
    bool started_;
    std::string name_;
    int numThreads_;

    int nextOpId_;
    FILEChannelMap channels_;
    EventLoopThreadPool threadPool_;

    FILEReadCallback readCompleteCallback_;
    FILEWriteCallback writeCompleteCallback_;

private:
    static void newWrite(FILEChannelPtr channel);
    static void newRead(FILEChannelPtr channel);
    void readCompleteCallback(FILEChannelPtr channel, TimeStamp timestamp);
    void writeCompleteCallback(FILEChannelPtr channel, TimeStamp timestamp);
};

#endif