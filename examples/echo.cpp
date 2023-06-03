#include "TCPServer.h"
#include "util.hpp"
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <glog/logging.h>

using namespace std;
int main(int argc, char **argv)
{
    // google::InitGoogleLogging(argv[0]);
    if (argc < 2) {
        LOG(WARNING) << "argc less than 2 \n";
        return -1;
    }
    sockaddr_in listenAddr = parseAddr(string(argv[1]));
    TCPServer server(listenAddr, "echo", 1);
    auto f = [](TCPConnPtr conn, void* buf, size_t sz, TimeStamp t) -> void {
        LOG(INFO) << conn->ConnName() << " echo " << sz << " bytes, "
                  << "data received at " << t;
        conn->Send(buf,sz);
    };
    server.SetMessageCallback(f);
    server.Start();
    return 0;
}