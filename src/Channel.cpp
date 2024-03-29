#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = READ;
const int Channel::kWriteEvent = WRITE;
const int Channel::kAcceptEvent = ACCEPT;

void Channel::update()
{
    loop_->UpdateChannel(this);
}

void Channel::Remove()
{
    loop_->RemoveChannel(this);
}

void TCPChannel::HandleEvent(int receiveTime)
{
    // if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
    //     if (closeCallback_)
    //         closeCallback_(receiveTime);
    // }
    // if (revents_ & (POLLERR | POLLNVAL)) {
    //     if (errorCallback_)
    //         errorCallback_(receiveTime);
    // }
    // if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    //     if (readCallback_)
    //         readCallback_(receiveTime);
    // }
    // if (revents_ & POLLOUT) {
    //     if (writeCallback_)
    //         writeCallback_(receiveTime);
    // }
    if (revents_ & (READ| ACCEPT)) {
        if (readCallback_)
            readCallback_(receiveTime);
    }
    if (revents_ & WRITE) {
        if (writeCallback_)
            writeCallback_(receiveTime);
    }
}

void FILEChannel::HandleEvent(int receiveTime)
{
    if (revents_ & READ) {
        if (readCallback_)
            readCallback_(receiveTime);
    }
    if (revents_ & WRITE) {
        if (writeCallback_)
            writeCallback_(receiveTime);
    }
}

std::string FILEChannel::FILEChannelName(){return name_;}
void FILEChannel::SetFileChannelName(const std::string& name){name_=name;}