#include "FILEOperator.h"
#include "LinuxHead.h"
#include <condition_variable>
#include <fcntl.h>
#include <glog/logging.h>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <vector>

#define RWRWRW (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)

std::vector<std::string> read_files { "read0", "read1", "read2", "read3", "read4", "read5", "read6", "read7", "read8", "read9" };
std::vector<std::string> write_files{"write0", "write1", "write2", "write3","write4","write5","write6","write7","write8","write9"};
std::string msg = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz"
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
int main()
{
    FILEOperator fileoperator("fop");
    std::atomic<int> count(0);
    auto readCallback = [&](void* buf, size_t len, TimeStamp t) -> void {
        LOG(INFO) << "readCallback with content " << (char*)buf;
        count++;
    };
    auto writeCallback = [&](TimeStamp t) -> void {
        LOG(INFO) << "writingCallback";
        count++;
    };
    fileoperator.SetReadCompleteCallback(readCallback);
    fileoperator.SetWriteCompleteCallback(writeCallback);
    fileoperator.Start();
    auto f = [&] {
        std::vector<int> read_fds;
        for (int i = 0; i < 10; i++) {
            int fd = open(read_files[i].c_str(), O_RDONLY);
            if (fd < 0) {
                LOG(WARNING) << "failed to open";
                return;
            }
            read_fds.push_back(fd);
            fileoperator.Read(fd);
        }
        std::vector<int> write_fds;
        for (int i = 0; i < 10; i++) {
            int fd = open(write_files[i].c_str(), O_RDWR | O_CREAT, RWRWRW);
            if (fd < 0)
                return;
            write_fds.push_back(fd);
            fileoperator.Write(fd, msg.c_str(), msg.length());
        }
        while (count != 20)
        {
            sleep(1);
        }
        for (int i = 0; i < 10; i++) {
            close(read_fds[i]);
            close(write_fds[i]);
        }
    };
    std::thread t(f);
    t.join();
    sleep(1);
    return 0;

}