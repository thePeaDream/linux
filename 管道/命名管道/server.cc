//服务端读取客户端发送的数据
#include "com.hpp"
#include "log.hpp"
#define PROCESS_NUM 3
using namespace std;
void getClientMessage(int fd)
{
    while(true)
    {
        char buffer[SIZE];
        memset(buffer,'\0',sizeof buffer);
        ssize_t s = read(fd,buffer,sizeof(buffer)-1);
        if(s == 0)
        {
            //写端关闭，读到文件结尾
            cout << "client write quit,server read quit too" << endl;
            break;
        }
        else if(s > 0)
        {
            //buffer里就是用户发过来的数据
            cout << "["<<getpid()<<"]"<<" client sent ："<< string(buffer) << endl;
        }
    }
}
int main()
{
    //1 创建命名管道文件
    mkfifo(pipe_path.c_str(),MODE);
    log("服务器创建命名管道文件成功",DEBUG) << endl;
    
    //2 打开命名管道文件
    int fd = open(pipe_path.c_str(),O_RDONLY);
    log("服务器打开命名管道文件成功",DEBUG) <<endl;
    //父进程创建一批子进程，让子进程来读取客户端的数据
    for(int i = 0; i < PROCESS_NUM;++i)
    {
        pid_t id = fork();
        if(id == 0)
        {
            //子进程
            //3 读取客户端的数据
            getClientMessage(fd);
            close(fd);
            exit(0);
        }
    }
    for(int i = 0; i < PROCESS_NUM;++i)
    {
        waitpid(-1,nullptr,0);
    }
    //4 关闭文件
    close(fd);
    log("管道文件关闭成功",DEBUG)<< endl;
    //通信完毕，删除管道文件
    unlink(pipe_path.c_str());
    log("管道文件已被删除",DEBUG)<< endl;
    return 0;
}
