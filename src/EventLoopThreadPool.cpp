#include "EventLoopThreadPool.h"
#include <cassert>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, int thread_num, const std::string& name)
    : started_(false)
    , baseLoop_(baseLoop)
    , thread_num_(thread_num)
    , name_(name)
    , next_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::Start()
{
    assert(!started_);
    started_ = true;
    for (int i = 0; i < thread_num_; i++) {
        EventLoopThread* t = new EventLoopThread(name_ + std::to_string(i));
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->Run());
    }
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
    assert(started_);
    EventLoop* loop = baseLoop_;
    if (!threads_.empty()) {
        loop = loops_[next_++];
        if (next_ == loops_.size())
                next_ = 0;
    }
    return loop;
}