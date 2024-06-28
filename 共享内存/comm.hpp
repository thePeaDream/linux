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
#define PATH_NAME "/home/lyh/newfile"
#define PROJ_ID 1
#define SHM_SIZE 4096 //共享内存的大小，最好是页(PAGE:4096)的整数倍

#endif