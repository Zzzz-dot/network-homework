#include "TCPServer.h"
#include "util.hpp"
#include <netinet/in.h>
#include <string>
#include <iostream>

using namespace std;
int main(int argc, char **argv)
{
    if (argc < 2)
        return -1;
    sockaddr_in listenAddr = parseAddr(string(argv[1]));
    TCPServer server(listenAddr, "echo", 1);
    auto f = [](TCPConnPtr conn, void* buf, size_t sz, TimeStamp t) -> void {
        cout << conn->ConnName() << " echo " << sz << " bytes, "
                 << "data received at " << t;
        conn->Send(buf,sz);
    };
    server.SetMessageCallback(f);
    server.Start();
    return 0;
}