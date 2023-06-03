#ifndef _LINUXHEAD_H
#define _LINUXHEAD_H
#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cassert>


class TCPConn;
using TimeStamp = int;
using TCPConnPtr = TCPConn*;
using EventCallback = std::function<void(TCPConnPtr, TimeStamp)>;
using ReadEventCallback = std::function<void(TCPConnPtr, void*, size_t, TimeStamp)>;

#define BUFCOUNT 1024
#define BUFSIZE 4096
#define DEFAULT_PORT 1234
#define URING_LENGTH 1024

#endif