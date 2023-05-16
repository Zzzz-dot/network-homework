#ifndef _URINGPOLLER_H
#define _URINGPOLLER_H

#include "Poller.h"

class UPoller : public Poller {
    /// Polls the I/O events.
    /// Must be called in the loop thread.
    virtual int Poll(int timeoutMs, ChannelList& activeChannels) override;

    /// Changes the interested I/O events.
    /// Must be called in the loop thread.
    virtual void UpdateChannel(Channel* channel) override;

    /// Remove the channel, when it destructs.
    /// Must be called in the loop thread.
    virtual void RemoveChannel(Channel* channel) override;
};

#endif