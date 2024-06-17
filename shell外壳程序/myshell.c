#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
//myshell接收的命令最大长度 —用于存放接收的命令
#define COMMAND_MAX_LENGTH 1024
//myshell命令行参数最大个数
#define ARGVNUM 32
//myshell命令的默认分割符号——空格
#define SPLIT " "
//用于存放接收的命令
char command[COMMAND_MAX_LENGTH];
//存放分割出的命令行参数
char* myargv[ARGVNUM];
extern char** environ;

//存放用户添加的环境变量，buffer里的每个环境变量用'\0'分割
char buffer[1024];
//存放用户添加环境变量的下一个位置
int POS = 0;

//shell运行原理：通过让子进程执行命令，父进程等待&&解析命令
int main()
{
    //命令行解释器，常驻内存不退出
    while(1)
    {
        //1 打印命令行相关信息
        printf("[lyh@localhost myshell#] ");
        fflush(stdout);
        memset(command,'\0',sizeof(command));
        //2 接收用户输入的命令/字符串
        if(fgets(command,sizeof(command),stdin) == NULL)
        { continue; }
        command[strlen(command)-1] = '\0';//注意：fgets会读取'\n' ——"ls -a -l\n\0"
        //3 用strtok分割命令 解析字符串
        myargv[0] = strtok(command,SPLIT);//第一次调用传入原始字符串，如果要继续解析原始字符串，后面传入NULL即可
        int i = 1;
        if(strcmp(myargv[0],"ls")==0)
        {
            //加颜色
            myargv[i++] = "--color=auto";
        }
        while(myargv[i++] = strtok(NULL,SPLIT))//不可分割后会返回NULL
        {
            if(i == ARGVNUM)
            {
                //能解析的命令行参数量达到上限
                break;
            }
        }
        //此时命令行参数都存放在myargv[]
        
        //4 特殊命令处理 子进程执行cd命令，不会影响父进程myshell的当前路径，所以需要特殊处理
        //让父进程bash自己执行的命令，叫内置命令 
        //内置命令本质是一个函数，并不是磁盘里的可执行程序  export
        if(strcmp(myargv[0],"cd") == 0)//不需要创建子进程，让父进程自己执行execlp("cd","cd","..");
        {
            if(myargv[1]!=NULL)
                chdir(myargv[1]);
            continue;
        }
        if(strcmp(myargv[0],"export") == 0 && myargv[1] != NULL)//给shell本身导入对应环境变量 例如:export a=99
        {
            //添加环境变量到父进程，实际上是将环境变量字符串的地址拷贝到environ[]中
            //虽然用strtok切割了子串，但是字符的地址依旧是基于command[]里的
            strcpy(buffer+POS,myargv[1]);
            char* newenv = buffer+POS;
            POS += (strlen(myargv[1])+1); 

            //添加一个环境变量
            int ret = putenv(newenv);
            if (ret == 0) printf("环境变量导入成功！！！\n");
            
            continue;
        }
        //5 创建子进程去执行任务
        pid_t id = fork();
        if(id == 0)
        {
            //子进程
            printf("让子进程执行:%s\n",myargv[0]);
            execvp(myargv[0],myargv);
            exit(5);
        }
        else if(id > 0)
        {
            int status = 0;
            //父进程
            waitpid(id,&status,0);//阻塞等待
        }
        else
        {
            //创建子进程失败
            continue;
        }

    }
    return 0;
}
