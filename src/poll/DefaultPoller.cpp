#include "Poller.h"
#include "poll/EpollPoller.h"
#include "poll/UringPoller.h"
#include <glog/logging.h>
#include <stdlib.h>

Poller* Poller::newDefaultPoller()
{
    if (::getenv("USE_URING")) {
        LOG(INFO)<<"Using URING environment variable";
        return new UringPoller();
    } else {
        LOG(INFO)<<"Using EPOLL environment variable";
        return new EpollPoller();
    }
}
