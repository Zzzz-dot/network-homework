# 项目介绍
- 模仿 muduo 项目，实现一个 Reactor 模式服务器模型，实现 EPOLL 以及 URING 两种 Poller
- 实现两个 Channel，分别是 TCPChannel 以及 FILEChannel
- 功能测试以及性能测试

## Reactor 模式
`Reactor` 模式也叫 `Dispatcher` 模式，即 I/O 多路复用（SELECT/POLL/EPOLL）监听事件，收到事件后，根据事件类型分配（Dispatch）给某个进程 / 线程。

`Reactor` 模式主要由 `Reactor` 和处理资源池这两个核心部分组成，它俩负责的事情如下：
1. Reactor 负责监听和分发事件，事件类型包含连接事件、读写事件；
2. 处理资源池负责处理事件，如 read -> 业务逻辑处理 -> send；

一个典型的 Reactor 服务器模式架构如下图所示：

应用程序里面有  Reactor、Acceptor、Handler 这三个对象：
- Reactor 对象的作用是监听和分发事件；
- Acceptor 对象的作用是获取连接；
- Handler 对象的作用是处理业务；

1. Reactor 对象通过 IO 多路复用接口监听事件，收到事件后通过 dispatch 进行分发，具体分发给 Acceptor 对象还是 Handler 对象，还要看收到的事件类型；
2. 如果是连接建立的事件，则交由 Acceptor 对象进行处理，Acceptor 对象会通过 accept 方法获取连接，并创建一个 Handler 对象来处理后续的响应事件；
3. 如果不是连接建立事件， 则交由当前连接对应的 Handler 对象来进行响应；
4. Handler 对象通过 read -> 业务处理 -> send 的流程来完成完整的业务流程。

### muduo 中的实现模式


## 系统实现

## 系统构建

## 系统评估



## 依赖项
- 本项目依赖于 [liburing](https://github.com/axboe/liburing)，它是对 io_uring 的封装，提供一个更方便得 uring 编程库；
- 本项目依赖 [glog](https://github.com/google/glog)，实现项目运行过程中的状态打印，跟踪服务器状态；