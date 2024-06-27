#ifndef _COM_HPP_
#define _COM_HPP_
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <cstring>
#define MODE 0666  //权限
#define SIZE 1024  //一次通信的最大数据量
//管道文件的路径
std::string pipe_path = "/home/lyh/pipeFile"; 
#endif