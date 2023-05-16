#include "Channel.h"
#include "EventLoop.h"
#include <poll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

void Channel::update()
{
    loop_->UpdateChannel(this);
}

void TCPChannel::HandleEvent(int receiveTime)
{
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if (closeCallback_)
            closeCallback_(receiveTime);
    }
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_)
            errorCallback_(receiveTime);
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_)
            readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT) {
        if (writeCallback_)
            writeCallback_(receiveTime);
    }
}