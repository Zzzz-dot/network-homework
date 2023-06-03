#include "poll/UringPoller.h"
#include "Channel.h"
#include "LinuxHead.h"
#include <asm-generic/errno-base.h>
#include <atomic>
#include <cerrno>
#include <cstring>
#include <glog/logging.h>
#include <liburing.h>
#include <linux/time_types.h>
#include <sys/socket.h>

// std::atomic<int> UringPoller::BGID(1);

UringPoller::UringPoller()
{
    if (int e=io_uring_queue_init(URING_LENGTH, &ring, 0) < 0) {
        LOG(FATAL) << "io_uring_queue_init failed: "<<strerror(e)<<"\n";
    }
}

UringPoller::~UringPoller()
{
    io_uring_queue_exit(&ring);
}

TimeStamp UringPoller::Poll(int timeoutMs, ChannelList& activeChannels)
{
    io_uring_cqe* cqe;
    struct __kernel_timespec ts = { 0, timeoutMs * (long long) 1e3 };
    sigset_t sigmask;
    int numEvents = io_uring_submit_and_wait_timeout(&ring, &cqe, 1, &ts, &sigmask);
    // io_uring_submit_and_wait(&ring, 1);
    int savedErrno = errno;
    TimeStamp now = 0;
    unsigned head;
    unsigned count = 0;
    io_uring_for_each_cqe(&ring, head, cqe)
    {
        ++count;
        conn_info conn_i;
        memcpy(&conn_i, &cqe->user_data, sizeof(conn_i));
        int fd=conn_i.fd;
        int type = conn_i.type;
        Channel* channel = channels_[fd];
        if (cqe->res < 0) {
            LOG(WARNING) << "type: " << type << ", cqe->res: " << cqe->res << " \n";
            if (cqe->res == -EINVAL)
                LOG(FATAL) << "Accept is not supported \n";
        }

        if (type == ACCEPT) {
            int n = cqe->res;
            if (n < 0) {
                fillActiveChannels(channel, ERROR, n, activeChannels);
            } else {
                fillActiveChannels(channel, ACCEPT, n, activeChannels);
                if(channel->IsEnableAccepting())
                    add_accept(channel, 0);
            }
        }
        else if (type == READ) {
            int n = cqe->res;
            if (n < 0) {
                fillActiveChannels(channel, ERROR, n,activeChannels);
            } else {
                fillActiveChannels(channel, READ, n, activeChannels);
                channel->Recv();
                if (n != 0 && channel->IsEnableReading())
                    add_read(channel, 0);
            }
        } else if (type == WRITE) {
            int n = cqe->res;
            if (n < 0) {
                fillActiveChannels(channel, ERROR, n, activeChannels);
            } else {
                // fillActiveChannels(channel, WRITE, n, activeChannels);
                // if (n < channel->WriteLen() && channel->IsEnableWriting()) {
                //     // ADD(channel, 0);
                // }
            }
        } else {
            LOG(WARNING) << "unknown type: " << type <<" \n";
        }
    }
    io_uring_cq_advance(&ring, count);
    return now;
}

void UringPoller::UpdateChannel(Channel* channel)
{
    int fd = channel->FD();
    if (channels_.find(fd) == channels_.end())
    {
        channels_[fd] = channel;
    }
    if (channel->IsEnableAccepting()) {
        add_accept(channel, 0);
    }
    if (channel->IsEnableReading()) {
        add_read(channel, 0);
    }
    if (channel->IsEnableWriting()) {
        add_write(channel, 0);
    }
    // if (channel->IsProvBuffer())
    // {
    //     add_provide_buffer(channel->GetBid());
    //     channel->SetBid(-1);
    //     channel->SetBuf(nullptr);
    // }
}

void UringPoller::RemoveChannel(Channel* channel)
{
    int fd = channel->FD();
    channels_.erase(fd);
}

void UringPoller::fillActiveChannels(Channel* channel, int revents, int n, ChannelList& activeChannels)
{
    channel->SetRevent(revents);
    channel->SetN(n);
    // channel->SetBid(bid);
    // if (bid != -1)
    //     channel->SetBuf(Buffers[bid]);
    activeChannels.push_back(channel);
}

// void UringPoller::add_provide_buffer(int bid)
// {
//     struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
//     io_uring_prep_provide_buffers(sqe, Buffers[bid], BUFSIZE, 1, bgid, bid);

//     conn_info conn_i = {
//         .channel = nullptr,
//         .type = PROV_BUFFER,
//         .bid = -1
//     };
//     memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
// }

void UringPoller::add_accept(Channel* channel, unsigned flags)
{
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    int fd = channel->FD();
    io_uring_prep_accept(sqe, fd, (sockaddr*)&peerAddr, &peerAddrLen, 0);
    io_uring_sqe_set_flags(sqe, flags);
    // sqe->buf_group = bgid;

    conn_info conn_i = {
        .fd = (__u32)fd,
        .type = ACCEPT,
    };
    // io_uring_sqe_set_data(sqe, &conn_i);
    memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
}

void UringPoller::add_read(Channel* channel, unsigned flags)
{
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    int fd = channel->FD();
    io_uring_prep_recv(sqe, fd, channel->NextReadBuf(), BUFSIZE, 0);
    io_uring_sqe_set_flags(sqe, flags);
    // sqe->buf_group = bgid;

    conn_info conn_i = {
        .fd = (__u32)fd,
        .type = READ,
    };
    // io_uring_sqe_set_data(sqe, &conn_i);
    memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
}

void UringPoller::add_write(Channel* channel, unsigned flags)
{
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    int fd = channel->FD();
    io_uring_prep_send(sqe, fd, channel->WriteBuf(), channel->WriteLen(), 0);
    io_uring_sqe_set_flags(sqe, flags);

    conn_info conn_i = {
        .fd = (__u32)fd,
        .type = WRITE,
    };
    // io_uring_sqe_set_data(sqe, &conn_i);
    memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
}