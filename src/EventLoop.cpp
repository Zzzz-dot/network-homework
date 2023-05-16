#include "EventLoop.h"
#include "poll/EpollPoller.h"
#include "Channel.h"

EventLoop::EventLoop()
    : looping_(false)
    , iterations_(0)
    , onwer_thread_id_(std::this_thread::get_id())
    , poller_(new EpollPoller()) {};

int EventLoop::timeoutMs_ = 1000;

void EventLoop::Loop()
{
    assert(!looping_);
    looping_ = true;
    while (looping_) {
        ChannelList activeChannels;
        int pollReturnTime = poller_->Poll(timeoutMs_, activeChannels);
        ++iterations_;
        for (auto c : activeChannels) {
            c->HandleEvent(pollReturnTime);
        }
        doPendingTask();
    }
}

void EventLoop::Quit()
{
    looping_ = false;
    WakeUp();
}

void EventLoop::WakeUp()
{
    // 另外使用一个 fd，使得 Poll 返回，这里不实现
}

void EventLoop::NewTask(const PendingTask& task)
{
    if (std::this_thread::get_id() == onwer_thread_id_)
        task();
    else {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            tasks_.push_back(std::move(task));
        }
        WakeUp();
    }
}

void EventLoop::doPendingTask()
{
    std::vector<PendingTask> tasks;
    {
        std::lock_guard<std::mutex> l(mutex_);
        tasks.swap(tasks_);
    }
    for (auto& task : tasks) {
        task();
    }
}

// 同一个 Loop 线程完成
void EventLoop::UpdateChannel(Channel* channel)
{
    assert(channel->OnwerLoop() == this);
    poller_->UpdateChannel(channel);
}

// 同一个 Loop 线程完成
void EventLoop::RemoveChannel(Channel* channel)
{
    assert(channel->OnwerLoop() == this);
    poller_->RemoveChannel(channel);
}