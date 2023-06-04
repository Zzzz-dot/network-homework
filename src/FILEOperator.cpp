#include "FILEOperator.h"
#include "Channel.h"
#include "util.hpp"
#include <cassert>
#include <glog/logging.h>
#include <string>

void FILEOperator::Start()
{
    if (!started_) {
        LOG(INFO) << "FILEOperator Start";
        started_ = true;
        threadPool_.Start();
    }
}

FILEOperator::FILEOperator(const std::string& name, int numThread)
    : started_(false)
    , name_(name)
    , numThreads_(numThread)
    , nextOpId_(0)
    , threadPool_(nullptr, numThreads_, name_)

{
    assert(numThread > 0);
}


void FILEOperator::Read(int fd)
{
    EventLoop* loop = threadPool_.GetNextLoop();
    std::string opName = name_ + "#" + std::to_string(fd) + "#Read" + std::to_string(nextOpId_);
    ++nextOpId_;
    LOG(INFO) << "New Read Request " << opName;
    FILEChannelPtr channel = new FILEChannel(fd, loop);
    channel->SetFileChannelName(opName);
    channel->SetReadCallback(std::bind(&FILEOperator::readCompleteCallback, this, channel, std::placeholders::_1));
    channels_[opName] = channel;
    loop->NewTask(std::bind(&FILEOperator::newRead, channel));
}

void FILEOperator::Write(int fd, const char* buf, int len)
{
    EventLoop* loop = threadPool_.GetNextLoop();
    std::string opName = name_ + "#" + std::to_string(fd) + "#Write" + std::to_string(nextOpId_);
    ++nextOpId_;
    LOG(INFO) << "New Write Request " << opName;
    FILEChannelPtr channel = new FILEChannel(fd, loop);
    channel->SetFileChannelName(opName);
    channel->SetWriteBuf((const void*)buf);
    channel->SetWriteLen(len);
    channel->SetWriteCallback(std::bind(&FILEOperator::writeCompleteCallback, this, channel, std::placeholders::_1));
    channels_[opName] = channel;
    loop->NewTask(std::bind(&FILEOperator::newWrite, channel));
}

void FILEOperator::newRead(FILEChannelPtr channel)
{
    channel->EnableReading();
}

void FILEOperator::newWrite(FILEChannelPtr channel)
{
    channel->EnableWriting();
}

void FILEOperator::readCompleteCallback(FILEChannelPtr channel, TimeStamp timestamp)
{
    LOG(INFO) << "Read Request Finish " << channel->FILEChannelName() << " at " << timestamp;
    char readbuf[BUFSIZ];
    int n;
    char* buf;
    if ((n = channel->GetN()) != -1) {
        buf = (char*)channel->ReadBuf();
    } else {
        buf = readbuf;
        n = read(channel->FD(), buf, BUFSIZ);
    }
    readCompleteCallback_(buf, n, timestamp);
    channel->Remove();
    channels_.erase(channel->FILEChannelName());
}

void FILEOperator::writeCompleteCallback(FILEChannelPtr channel, TimeStamp timestamp)
{
    LOG(INFO) << "Write Request Finish " << channel->FILEChannelName() << " at " << timestamp;
    writeCompleteCallback_(timestamp);
    channel->Remove();
    channels_.erase(channel->FILEChannelName());
}
