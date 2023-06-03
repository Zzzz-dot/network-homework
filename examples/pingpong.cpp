#include "util.hpp"
#include <LinuxHead.h>
#include <algorithm>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
using namespace std;
string msg = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
             "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";

int main(int argc, char** argv)
{
    int client_fd;
    if (argc < 2) {
        return -1;
    }
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }
    sockaddr_in remoteAddr = parseAddr(string(argv[1]));
    sockaddr_in localAddr;
    if (connect(client_fd, (sockaddr*)&remoteAddr, sizeof(sockaddr_in)) < 0) {
        return 1;
    }
    int count = 0;
    int rtt_min = 1e9;
    int rtt_max = 0;
    bool running=true;
    auto pingpong = [&] () {
        char buf[BUFSIZE];
        char recvBuf[BUFSIZE];
        memcpy(buf, msg.c_str(), msg.length());

        while (running) {
            auto start = chrono::steady_clock::now();
            int len = send(client_fd, buf, msg.length(),0);
            if (len < 0 || len!=msg.length()) {
                cout << "ping pong failed" << endl;
                break;
            }
            int nRecv = recv(client_fd, recvBuf, BUFSIZE, 0);
            if (nRecv < 0 || nRecv != msg.length()) {
                cout << "ping pong failed" << endl;
                break;
            }
            auto end = chrono::steady_clock::now();
            int rtt = chrono::duration_cast<chrono::microseconds>(end - start).count();
            rtt_min = min(rtt_min, rtt);
            rtt_max = max(rtt_max, rtt);
            count++;
        }
    };
    thread t(pingpong);
    sleep(10);
    running = false;
    t.join();
    close(client_fd);
    cout << "RTT_min: " << rtt_min << " us" << endl;
    cout << "RTT_max: " << rtt_max << " us" << endl;
    cout << "Count: " << count << endl;
    cout << "Throughput: " << msg.length() * count / 10 << " B/s" << endl;
    cout << "Throughput: " << msg.length() * count / 10 / 1024 << " KB/s" << endl;
    cout << "Throughput: " << msg.length() * count / 10 / 1024 / 1024 << "MB/s" << endl;
    return 0;
}