#ifndef _CHANNEL_H
#define _CHANNEL_H
#include <functional>

// FileChannel 两个事件：可读和可写
// TCPChannel 四个事件：可读、可写、关闭、错误
class EventLoop;
enum ChannelType { TCPType,
    FILEType };
enum {
    PROV_BUFFER,
    READ,
    WRITE,
    ERROR,
};
class Channel {
public:
    using ChannelEventCallback = std::function<void(int, int, void*)>;
    Channel(int fd, EventLoop* loop)
        : events_(0)
        , fd_(fd)
        , state_(-1)
        , loop_(loop)
        , revents_(0)
        , bid_(-1) {};

    ChannelType Type() const { return type; }

    virtual void HandleEvent(int receiveTime) = 0;
    void SetReadCallback(ChannelEventCallback cb)
    {
        readCallback_ = cb;
    }
    void SetWriteCallback(ChannelEventCallback cb)
    {
        writeCallback_ = cb;
    }
    void SetCloseCallback(ChannelEventCallback cb)
    {
        closeCallback_ = cb;
    }
    void SetErrorCallback(ChannelEventCallback cb)
    {
        errorCallback_ = cb;
    }
    bool IsEnableReading()
    {
        return events_ & READ;
    }
    void EnableReading()
    {
        events_ |= kReadEvent;
        update();
    }
    void DisableReading()
    {
        events_ &= ~kReadEvent;
        update();
    }
    bool IsEnableWriting()
    {
        return events_ & WRITE;
    }
    void EnableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }
    void DisableWriting()
    {
        events_ &= ~kWriteEvent;
        update();
    }
    bool IsProvBuffer()
    {
        return events_ & PROV_BUFFER;
    }
    void ProvBuffer()
    {
        events_ |= PROV_BUFFER;
        update();
        events_ &= ~PROV_BUFFER;
    }
    void Remove();

    int Events() const { return events_; }
    void SetRevent(int event)
    {
        revents_ = event;
    }
    void SetBid(int bid) { bid_ = bid; }
    int GetBid() const { return bid_; }
    void SetBuf(void* buf) { buf_ = buf; }
    void* GetBuf() { return buf_; }
    int FD() const { return fd_; }
    int State() const { return state_; }
    void SetState(int state) { state_ = state; }
    EventLoop* OnwerLoop()
    {
        return loop_;
    }

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
    int events_;

    int fd_;
    int state_;
    EventLoop* loop_;

    void update();

protected:
    ChannelType type;
    int revents_;
    int bid_;
    void* buf_;
    ChannelEventCallback readCallback_;
    ChannelEventCallback writeCallback_;
    ChannelEventCallback closeCallback_;
    ChannelEventCallback errorCallback_;
};

class TCPChannel : public Channel {
public:
    TCPChannel(int fd, EventLoop* loop)
        : Channel(fd, loop)
    {
        type = ChannelType::TCPType;
    }
    void HandleEvent(int receiveTime) override;
};

#endif