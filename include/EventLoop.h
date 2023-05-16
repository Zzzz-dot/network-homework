#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H
#include "Poller.h"
#include <memory>
#include <mutex>
#include <thread>
// 管理一个Poller
// vector<PendingFunction> 后续需要处理的函数
// Channels
class EventLoop {
public:
    EventLoop();
    ~EventLoop()=default;
    using PendingTask = std::function<void()>;
    void Loop();
    void Quit();
    void WakeUp();
    void NewTask(const PendingTask& task);
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);

private:
    using ChannelList = std::vector<Channel*>;
    bool looping_;
    int iterations_;
    const std::thread::id onwer_thread_id_;
    std::unique_ptr<Poller> poller_;
    std::mutex mutex_;
    std::vector<PendingTask> tasks_;

    static int timeoutMs_;

private:
    void doPendingTask();
};
#endif