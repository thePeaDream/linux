#ifndef COMM_HPP_
#define COMM_HPP_
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h> //Inter-Process Communication
#include <sys/shm.h>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include "log.hpp"
#define PATH_NAME "/home/lyh/newfile"
#define PIPE_NAME "/home/lyh/fifo"
#define PROJ_ID 1
#define SHM_SIZE 4096 //共享内存的大小，最好是页(PAGE:4096)的整数倍

//在初始化时，创建管道文件
class Init
{
public:
    Init()
    {
        umask(0);
        int n = mkfifo(PIPE_NAME,0666);
        log("create pipe file done",DEBUG) << endl;
    }
    ~Init()
    {
        //删除对应管道文件
        unlink(PIPE_NAME);
        log("delete pipe file done",DEBUG)<<endl;
    }

};

#define READ O_RDONLY
#define WRITE O_WRONLY
//打开管道文件
int OpenPipe(const string& pathname,int flags)
{
    int fd = open(pathname.c_str(),flags);
    assert(fd >= 0);
    return fd;
}

void Wait(int fd)
{
    log("等待中......",DEBUG)<<endl;
    uint32_t tmp = 0;
    ssize_t s = read(fd,&tmp,sizeof(uint32_t));
}

void Wake(int fd)
{
    log("唤醒中.....",DEBUG)<<endl;
    uint32_t tmp = 1;
    ssize_t s = write(fd,&tmp,sizeof(tmp));
}
void ClosePipe(int fd)
{
    close(fd);
}
#endif