#include "comm.hpp"
#include "log.hpp"
int main()
{
    //1 生成与服务端相同的key
    key_t key = ftok(PATH_NAME,PROJ_ID);
    assert(key != -1);
    log("create key done",DEBUG) << " client key is：" << key << endl;
    //2 用相同的key找到服务端创建的共享内存
    int shmid = shmget(key,SHM_SIZE,IPC_CREAT);
    assert(shmid != -1);
    log("get shmid done",DEBUG) << " shmid is：" << shmid << endl;

    //3 将共享内存关联到客户端地址空间（获取共享内存）
    char* shmaddr = (char*)shmat(shmid,nullptr,0);
    assert(shmaddr != nullptr);
    log("attach shm done",DEBUG) << " shmid is: " << shmid << endl;
    
    //int count = 10;
    // 具体的通信逻辑
   // while(count--)
   // {
   //     //每次都向addr[共享内存的起始地址]写入
   //     snprintf(addr,SHM_SIZE,"hello server,i am client process,pid : %d,count : %d",getpid(),count);    
   //     sleep(1);
   // }
   // snprintf(addr,SHM_SIZE,"quit");

   int fd = OpenPipe(PIPE_NAME,WRITE);

   while(true)
   {
       //从键盘中读到的数据，直接放入共享区中,注意'\n'
       ssize_t r = read(0,shmaddr,SHM_SIZE-1);
       assert(r != 0);
       shmaddr[r-1] = '\0';
       if(strcmp(shmaddr,"quit") == 0) break;
       //写入成功，唤醒对方
       Wake(fd);
   }
   ClosePipe(fd);
    //4 将共享内存和客户端去关联
    shmdt(shmaddr);
    log("detach shm done",DEBUG) << "shmid is " << shmid << endl;
    return 0;
}
