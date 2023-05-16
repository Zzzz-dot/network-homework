#ifndef _EVENTLOOPTHREAD_H
#define _EVENTLOOPTHREAD_H
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
class EventLoop;
// 管理 Thread 以及 EventLoop
class EventLoopThread {
public:
    EventLoopThread(const std::string& name);
    ~EventLoopThread();
    EventLoop* Run();

private:
    bool running_;
    std::string name_;
    std::mutex mutex_;
    std::condition_variable cond_;
    EventLoop* loop_;
    std::thread thread_;
    void threadFunc();
};

#endif