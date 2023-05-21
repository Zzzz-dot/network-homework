#include "poll/EpollPoller.h"
#include "Channel.h"
#include <cstdio>
#include <poll.h>
#include <sys/epoll.h>
#include <glog/logging.h>
#include <unistd.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EpollPoller::EpollPoller():epollfd_(epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize)
{

}

EpollPoller::~EpollPoller()
{
    close(epollfd_);
}

TimeStamp EpollPoller::Poll(int timeoutMs, ChannelList& activeChannels)
{
    int numEvents = epoll_wait(epollfd_,
        &*events_.begin(),
        static_cast<int>(events_.size()),
        timeoutMs);
    int savedErrno = errno;
    TimeStamp now=0;
    if (numEvents > 0) {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) {
        LOG(INFO)<<"EpollPoller time out after "<<timeoutMs<<" ms\n";
        // nothing to do
    } else {
        LOG(ERROR) << "EpollPoller error " << strerror(savedErrno) << " \n";
    }
    return now;
}

void EpollPoller::fillActiveChannels(int numEvents,
    ChannelList& activeChannels) const
{
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->SetRevent(events_[i].events);
        activeChannels.push_back(channel);
    }
}

void EpollPoller::UpdateChannel(Channel* channel)
{
    const int state = channel->State();
    if (state == kNew || state == kDeleted) {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->FD();
        if (state == kNew) {
            channels_[fd] = channel;
        }
        channel->SetState(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->FD();
        update(EPOLL_CTL_MOD, channel);
    }
}

void EpollPoller::RemoveChannel(Channel* channel)
{
    int fd = channel->FD();
    channels_.erase(fd);
    update(EPOLL_CTL_DEL, channel);
    channel->SetState(kDeleted);
}

void EpollPoller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    memset(&event,0,sizeof(epoll_event));
    event.events = channel->Events();
    event.data.ptr = channel;
    int fd = channel->FD();
    if (epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        //error
    }
}
