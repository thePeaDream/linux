#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <fcntl.h>
int main()
{
    //1 创建管道
    //int pipe(int pipefd[2]) 参数是输出型参数，自己接收被打开的文件fd
    int pipefd[2] = { 0 };
    int n = pipe(pipefd); //pipefd[0]:读端  pipefd[1]:写端
    assert(n != -1);

    //2 创建子进程
    pid_t id = fork();
    //3 构建单向通信的信道！！！！父进程写入，子进程读取
    if(id < 0)
    {
        //创建子进程失败！！！
        perror("fork");
        exit(1);
    }
    else if( id == 0)
    {
        //子进程
        //关闭写端pipefd[1],保留读端
        close(pipefd[1]);
        //读取要有一个缓冲区，用来保存读到的数据
        char buffer[1024];
        memset(buffer,0,sizeof buffer);
        while(true)
        {
            ssize_t num = read(pipefd[0],buffer,sizeof(buffer) - 1);
            if(num > 0)
            {
                buffer[num] = '\0';
                std::cout << "子进程pid:"<< getpid() <<"读取父进程的数据:" << buffer << std::endl;
            }
        }
        //通信完毕
        close(pipefd[0]);
    }
    else if(id > 0)
    {
        //父进程
        //关闭读端pipefd[0],保留写端
        close(pipefd[0]);
        //父进程写入
        std::string str = "父进程正在发消息";
        int count = 0;
        char sent_buffer[1024];
        while(true)
        {
            //向字符串里，格式化显示
            snprintf(sent_buffer,sizeof(sent_buffer),"%s : %d, 父进程pid是%d",str.c_str(),count++,getpid());
            //将sent_buffer写入到管道
            write(pipefd[1],sent_buffer,strlen(sent_buffer));
            sleep(2);
        }
        //阻塞等待
        pid_t ret = waitpid(id,nullptr,0);
        assert(ret);

        //通信完毕
        close(pipefd[1]);
    }
    return 0;
}