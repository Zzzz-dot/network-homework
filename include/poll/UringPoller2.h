// #ifndef _URINGPOLLER_H
// #define _URINGPOLLER_H

// #include "LinuxHead.h"
// #include "Poller.h"
// #include <atomic>
// #include <liburing.h>

// class UringPoller2 : public Poller {
// public:
//     UringPoller2();
//     ~UringPoller2();
//     /// Polls the I/O events.
//     /// Must be called in the loop thread.
//     virtual TimeStamp Poll(int timeoutMs, ChannelList& activeChannels) override;

//     /// Changes the interested I/O events.
//     /// Must be called in the loop thread.
//     virtual void UpdateChannel(Channel* channel) override;

//     /// Remove the channel, when it destructs.
//     /// Must be called in the loop thread.
//     virtual void RemoveChannel(Channel* channel) override;

// private:
//     static const int kInitEventListSize = 16;

//     static const char* operationToString(int op);

//     void fillActiveChannels(int numEvents, ChannelList& activeChannels) const;
//     void update(int operation, Channel* channel);

//     typedef std::vector<epoll_event> EventList;

//     int epollfd_;
//     io_uring ring;
//     EventList events_;
// };



// #endif