#include "poll/UringPoller.h"
#include "Channel.h"
#include "LinuxHead.h"
#include <atomic>
#include <cerrno>
#include <cstring>
#include <glog/logging.h>
#include <liburing.h>
#include <linux/time_types.h>

std::atomic<int> UringPoller::BGID(1);

UringPoller::UringPoller()
{
    if (int e=io_uring_queue_init(URING_LENGTH, &ring, 0) < 0) {
        LOG(FATAL) << "io_uring_queue_init failed: "<<strerror(e)<<"\n";
    }
    io_uring_sqe* sqe;
    io_uring_cqe* cqe;
    sqe = io_uring_get_sqe(&ring);
    bgid = BGID++;
    io_uring_prep_provide_buffers(sqe, Buffers, BUFSIZE, BUFCOUNT, bgid, 0);
    io_uring_submit(&ring);
    io_uring_wait_cqe(&ring, &cqe);
    if (cqe->res < 0) {
        LOG(FATAL) << "io_uring_task failed, ceq->res=" << cqe->res << "\n";
    }
    io_uring_cqe_seen(&ring, cqe);
}

UringPoller::~UringPoller()
{
    io_uring_queue_exit(&ring);
}

TimeStamp UringPoller::Poll(int timeoutMs, ChannelList& activeChannels)
{
    io_uring_cqe* cqe;
    struct __kernel_timespec ts;
    ts.tv_nsec = timeoutMs << 6;
    sigset_t sigmask;
    int numEvents = io_uring_submit_and_wait_timeout(&ring, &cqe, 1, &ts, &sigmask);
    int savedErrno = errno;
    TimeStamp now = 0;
    unsigned head;
    io_uring_for_each_cqe(&ring, head, cqe)
    {
        conn_info conn_i;
        memcpy(&conn_i, &cqe->user_data, sizeof(conn_i));
        int type = conn_i.type;
        Channel* channel = conn_i.channel;
        if (cqe->res == -ENOBUFS) {
            LOG(FATAL) << "buffers is empty, this should not happen\n";
        } else if (type == PROV_BUFFER) {
            if (cqe->res < 0)
                LOG(FATAL) << "cqe->res = " << cqe->res << "\n";
        } else if (type == READ) {
            int bytes_read = cqe->res;
            int bid = cqe->flags >> 16;
            if (cqe->res < 0) {
                add_provide_buffer(bid);
                fillActiveChannels(channel, ERROR, -1,activeChannels);
            } else {
                fillActiveChannels(channel, READ, bid, activeChannels);
                if (cqe->res != 0 && channel->IsEnableReading())
                    add_provide_read(channel, IOSQE_BUFFER_SELECT);
            }
        }
    }
    return now;
}

void UringPoller::UpdateChannel(Channel* channel)
{
    int fd = channel->FD();
    if (channels_.find(fd) == channels_.end())
    {
        channels_[fd]=channel;
    }
    if (channel->IsProvBuffer())
    {
        add_provide_buffer(channel->GetBid());
        channel->SetBid(-1);
        channel->SetBuf(nullptr);
    }
}

void UringPoller::RemoveChannel(Channel* channel)
{
    int fd = channel->FD();
    channels_.erase(fd);
}

void UringPoller::fillActiveChannels(Channel* channel, int revents, int bid, ChannelList& activeChannels)
{
    channel->SetRevent(revents);
    channel->SetBid(bid);
    if (bid != -1)
        channel->SetBuf(Buffers[bid]);
    activeChannels.push_back(channel);
}

void UringPoller::add_provide_buffer(int bid)
{
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_provide_buffers(sqe, Buffers[bid], BUFSIZE, 1, bgid, bid);

    conn_info conn_i = {
        .channel = nullptr,
        .type = PROV_BUFFER,
        .bid = -1
    };
    memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
}

void UringPoller::add_provide_read(Channel* channel, unsigned flags)
{
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    int fd = channel->FD();
    io_uring_prep_recv(sqe, fd, NULL, BUFSIZE, 0);
    io_uring_sqe_set_flags(sqe, flags);
    sqe->buf_group = bgid;

    conn_info conn_i = {
        .channel = channel,
        .type = READ,
        .bid = -1
    };
    memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
}

void UringPoller::add_provide_write(Channel* channel,void* buf, unsigned flags)
{
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    int fd = channel->FD();
    io_uring_prep_send(sqe, fd, buf, BUFSIZE, 0);
    io_uring_sqe_set_flags(sqe, flags);

    conn_info conn_i = {
        .channel = channel,
        .type = WRITE,
        .bid = -1,
    };
    memcpy(&sqe->user_data, &conn_i, sizeof(conn_i));
}