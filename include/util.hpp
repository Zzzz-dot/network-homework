#ifndef _UTIL_H
#define _UTIL_H
#include "LinuxHead.h"

static bool hasPort(const std::string& s)
{
    size_t index = s.find(":");
    if (index == -1) {
        return false;
    }
    return true;
}

static void ensurePort(std::string& s, uint16_t port)
{
    if (hasPort(s)) {
        return;
    } else {
        s += ":" + std::to_string(port);
    }
}

static sockaddr_in parseAddr(const std::string& s)
{
    // cluster_add format: [name/]<ip>[:port]
    int index = s.find("/");
    std::string nodeName = "";
    std::string host;
    // if cluster_addr contains nodename
    // but nodename is not used in this implemention
    if (index != -1) {
        nodeName = s.substr(0, index);
        host = s.substr(index);
    } else {
        host = s;
    }

    ensurePort(host, DEFAULT_PORT);
    index = host.find(":");

    sockaddr_in cluster_addr;
    bzero((void*)&cluster_addr, sizeof(sockaddr_in));
    cluster_addr.sin_family = AF_INET;
    cluster_addr.sin_port = htons(stoul(host.substr(index + 1)));
    if (int e = inet_pton(AF_INET, host.substr(0, index).c_str(), &cluster_addr.sin_addr) <= 0) {
        errno = e;
    }
    return cluster_addr;
}

static std::string convertAddr(const sockaddr_in& addr)
{
    char ip[INET_ADDRSTRLEN];
    const char* ptr = inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
    std::string ip_port = std::string(ptr) + ":" + std::to_string(addr.sin_port);
    return ip_port;
}

#endif