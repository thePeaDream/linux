//父进程创建一批子进程，在一张表里，选择让哪个子进程执行任务
#include "task.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <vector>
#define PROCESS_NUM 3//创建的子进程个数
int waitTask(int readfd,bool& quit)
{
    int32_t command;
    ssize_t ret = read(readfd,&command,sizeof(command));
    if(ret == 0)
    {
        quit = true;
        return -1;
    }
    assert(ret == sizeof(int32_t));
    return command; 
}
 
int main()
{
    srand(time(nullptr));
    loadTask();
    //放的是 子进程id : pipefd写端fd 的键值对

    vector<pair<pid_t,int>> pool;
    //创建多个进程
    for(int i = 0; i < PROCESS_NUM;++i)
    {
        int pipefd[2] = { 0 };
        int n = pipe(pipefd);
        assert(n == 0);
        //创建子进程
        pid_t id = fork();
        assert(id >= 0);
        //子进程让它等待读取
        if(id == 0)
        {
            //子进程关闭写端
            close(pipefd[1]);
            while(true)
            {
                bool quit = false;
                //等待读取父进程指令，等不到就阻塞
                int command = waitTask(pipefd[0],quit);
                if(quit) break;
                if(command >= 0 && command < TASK_VOLUME)
                {
                    //执行对应方法
                    taskContainer[command]();
                }
                else{
                    cout << "非法命令"<<endl;
                }       
            }
            close(pipefd[0]);
            exit(0);
        }
        //父进程
        pool.push_back(std::pair<pid_t,int>(id,pipefd[1]));
        //父进程关闭读端
        close(pipefd[0]);
    }
    //此时父进程有了PROCESS_NUM个进程，同时跟这些进程建立了管道
    //pool里可以 对任意一个子进程 通过写端fd 进行写入

    //分配任务给子进程 负载均衡
    while(true)
    {
        cout << "###############################################"<<endl;
        cout << "0 exit          1 showTask         2 assignTask"<<endl;
        cout << "###############################################"<<endl;
        cout << "请选择 >";
        int select;
        cin >> select;
        if(select == 1) showTask();
        else if(select == 2)
        {
            cout << "#############################################" << endl;
            cout << "0 读取MySQL                             1 调用" <<endl;
            cout << "                    2 保存                    " <<endl;
            cout << "#############################################" << endl;
            cout << "请选择>";
            //选择任务
            int command;
            cin >> command;
            //选择进程
            int choice = rand()%PROCESS_NUM;
            int procId = pool[choice].first;
            //管道的写端
            int writefd = pool[choice].second;
            if(command >=0 && command < taskContainer.size())
            {
                //把任务指派给指定进程
                assignTask(procId,writefd,command);
            }
        }   
        else if(select == 0)//关闭fd，所有的子进程都会退出，同时回收所有子进程数据
        {
            for(const pair<pid_t,int>& item:pool)
            {
                //写端关闭，对应读端的子进程会退出
                close(item.second);
            }
            for(const pair<pid_t,int>& item:pool)
            {
                //等待释放退出的子进程资源
                waitpid(item.first,nullptr,0);
            }
            break;
        }
    }

    return 0;
}
