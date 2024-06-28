#include "comm.hpp"
#include "log.hpp"
Init init;//在加载时，会自动构建全局变量，就要调用该类的构造函数 -----创建管道文件  当进程退出时，全局变量会被析构，自动调用析构函数，会自动删除管道文件
int main()
{
    //1 服务端用PATH_NAME和PROJ_ID创建唯一的key值，客户端也生成相同的key值，保证能指向同一共享内存的前提
    key_t key = ftok(PATH_NAME,PROJ_ID);
    assert(key != -1);
    log("create key done",DEBUG) << " server key is："<< key << endl;

    //2 创建共享内存 —— 建议要创建一个全新的共享内存 —— 通信的发起者
    int shmid = shmget(key,SHM_SIZE,IPC_CREAT | IPC_EXCL| 0666);//0666操作该共享内存的权限perm
    if(shmid == -1)
    {
        perror("shmget");
        exit(1);
    }
    log("create shm done",DEBUG) << " shmid："<< shmid << endl;
    
    //3 将指定的共享内存，关联到自己的地址空间
    //以读写方式0,得到共享内存的起始地址
    char* shmaddr = (char*)shmat(shmid,nullptr,0);
    log("attach shm done",DEBUG) <<  endl;
    
    int fd = OpenPipe(PIPE_NAME,READ);
    
    //这里就是通信的逻辑，将共享内存当成一个数组
    while(true)
    {
        //先等管道有内容时，再去读取内存块的内容 （访问控制）
        Wait(fd);
        //server每隔1秒读取该共享内存块的内容
        printf("%s\n",shmaddr);
        if(strcmp(shmaddr,"quit") == 0)
            break;
        //sleep(1);
    }
    ClosePipe(fd);

    //4 将指定的共享内存，从自己的地址空间去关联
    shmdt(shmaddr);
    log("detach shm done",DEBUG) << " shmid："<< shmid << endl;
    
    //5 删除共享内存
    shmctl(shmid,IPC_RMID,nullptr);
    log("delete shm done",DEBUG) << " shmid：" << shmid << endl;
    return 0;
}
