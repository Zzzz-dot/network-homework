#ifndef _EPOLLPOLLER_H
#define _EPOLLPOLLER_H

#include "Poller.h"

struct epoll_event;

class EpollPoller : public Poller {
public:
    EpollPoller();
    ~EpollPoller() override;

    TimeStamp Poll(int timeoutMs, ChannelList& activeChannels) override;
    void UpdateChannel(Channel* channel) override;
    void RemoveChannel(Channel* channel) override;

private:
    static const int kInitEventListSize = 16;

    static const char* operationToString(int op);

    void fillActiveChannels(int numEvents, ChannelList& activeChannels) const;
    void update(int operation, Channel* channel);

    typedef std::vector<epoll_event> EventList;

    int epollfd_;
    EventList events_;
};

#endif
