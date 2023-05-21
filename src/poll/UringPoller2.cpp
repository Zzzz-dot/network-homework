// #include "Channel.h"
// #include "poll/UringPoller2.h"
// #include <cstdio>
// #include <glog/logging.h>
// #include <poll.h>
// #include <sys/epoll.h>
// #include <unistd.h>

// const int kNew = -1;
// const int kAdded = 1;
// const int kDeleted = 2;

// UringPoller2::UringPoller2()
//     : epollfd_(epoll_create1(EPOLL_CLOEXEC))
//     , events_(kInitEventListSize)
// {
//     if (io_uring_queue_init(URING_LENGTH, &ring, 0) < 0) {
//         LOG(FATAL) << "io_uring_queue_init failed\n";
//     }
// }

// UringPoller2::~UringPoller2()
// {
//     close(epollfd_);
// }

// TimeStamp UringPoller2::Poll(int timeoutMs, ChannelList& activeChannels)
// {
//     int numEvents = epoll_wait(epollfd_,
//         &*events_.begin(),
//         static_cast<int>(events_.size()),
//         timeoutMs);
//     int savedErrno = errno;
//     TimeStamp now = 0;
//     if (numEvents > 0) {
//         fillActiveChannels(numEvents, activeChannels);
//         if (static_cast<size_t>(numEvents) == events_.size()) {
//             events_.resize(events_.size() * 2);
//         }
//     } else if (numEvents == 0) {
//         LOG(INFO) << "UringPoller2 time out after " << timeoutMs << " ms\n";
//         // nothing to do
//     } else {
//         LOG(ERROR) << "UringPoller2 error " << strerror(savedErrno) << " \n";
//     }
//     return now;
// }

// void UringPoller2::fillActiveChannels(int numEvents,
//     ChannelList& activeChannels) const
// {
//     for (int i = 0; i < numEvents; ++i) {
//         Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
//         channel->SetRevent(events_[i].events);
//         activeChannels.push_back(channel);
//     }
// }

// void UringPoller2::UpdateChannel(Channel* channel)
// {
//     const int state = channel->State();
//     if (state == kNew || state == kDeleted) {
//         int fd = channel->FD();
//         if (state == kNew) {
//             channels_[fd] = channel;
//         }
//         channel->SetState(kAdded);
//         update(EPOLL_CTL_ADD, channel);
//     } else {
//         int fd = channel->FD();
//         update(EPOLL_CTL_MOD, channel);
//     }
// }

// void UringPoller2::RemoveChannel(Channel* channel)
// {
//     int fd = channel->FD();
//     channels_.erase(fd);
//     update(EPOLL_CTL_DEL, channel);
//     channel->SetState(kDeleted);
// }

// void UringPoller2::update(int operation, Channel* channel)
// {
//     struct epoll_event event;
//     memset(&event, 0, sizeof(epoll_event));
//     event.events = channel->Events();
//     event.data.ptr = channel;
//     int fd = channel->FD();
//     io_uring_sqe* sqe = io_uring_get_sqe(&ring);
//     io_uring_prep_epoll_ctl(sqe, epollfd_, fd, operation, &event);
//     if (epoll_ctl(epollfd_, operation, fd, &event) < 0)
//     {
//         //error
//     }
// }