#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread(const std::string& name)
    : running_(false)
    , name_(name)
{
}

EventLoopThread::~EventLoopThread()
{
    running_ = false;
    if (loop_) {
        loop_->Quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::Run()
{
    assert(!running_);
    running_ = true;
    thread_ = std::thread(std::bind(&EventLoopThread::threadFunc, this));
    EventLoop* loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] { return this->loop_ != nullptr; });
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        pthread_setname_np(pthread_self(), name_.c_str());
        cond_.notify_one();
    }
    loop.Loop();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}