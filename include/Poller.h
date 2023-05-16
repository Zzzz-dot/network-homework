#ifndef _POLLER_H
#define _POLLER_H
#include <map>
#include <vector>
#include "LinuxHead.h"

class Channel;
class Poller {
public:
    Poller(){};
    virtual ~Poller(){};
    using ChannelList = std::vector<Channel*>;
    /// Polls the I/O events.
    /// Must be called in the loop thread.
    virtual TimeStamp Poll(int timeoutMs, ChannelList& activeChannels) = 0;

    /// Changes the interested I/O events.
    /// Must be called in the loop thread.
    virtual void UpdateChannel(Channel* channel) = 0;

    /// Remove the channel, when it destructs.
    /// Must be called in the loop thread.
    virtual void RemoveChannel(Channel* channel) = 0;

protected:
    using ChannelMap = std::map<int, Channel*>;
    ChannelMap channels_;
};

#endif