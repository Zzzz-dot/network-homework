#ifndef _URINGPOLLER_H
#define _URINGPOLLER_H

#include "LinuxHead.h"
#include "Poller.h"
#include <atomic>
#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>

class UringPoller : public Poller {
public:
    UringPoller();
    ~UringPoller();
    /// Polls the I/O events.
    /// Must be called in the loop thread.
    virtual TimeStamp Poll(int timeoutMs, ChannelList& activeChannels) override;

    /// Changes the interested I/O events.
    /// Must be called in the loop thread.
    virtual void UpdateChannel(Channel* channel) override;

    /// Remove the channel, when it destructs.
    /// Must be called in the loop thread.
    virtual void RemoveChannel(Channel* channel) override;

private:
    static const int kInitListSize = 16;
    io_uring ring;
    using EventList = std::vector<epoll_event>;

    // char Buffers[BUFCOUNT][BUFSIZE];

    // static std::atomic<int> BGID;
    // int bgid;
    sockaddr_in peerAddr;
    socklen_t peerAddrLen;

    struct conn_info {
        __u32 fd;
        __u32 type;
    };

    void fillActiveChannels(Channel* channel, int revents, int n, ChannelList& activeChannels);
    void add_accept(Channel* channel, unsigned flags);
    void add_read(Channel* channel, unsigned flags);
    void add_write(Channel* channel, unsigned flags);
    // void add_provide_buffer(int bid);
};



#endif