#ifndef _EVENTLOOPTHREADPOLL_H_
#define _EVENTLOOPTHREADPOLL_H
#include <string>
#include <vector>
#include <memory>
#include "EventLoopThread.h"
class EventLoop;
// 管理所有的 EventLoopThread 以及其下的 EventLoop
// 每次返回一个 EventloopT
class EventLoopThreadPool {
public:
    EventLoopThreadPool(EventLoop* base_loop, int thread_num, const std::string& name);
    ~EventLoopThreadPool();
    void Start();
    EventLoop* GetNextLoop();

private:
    bool started_;
    EventLoop* baseLoop_;
    int thread_num_;
    std::string name_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

#endif