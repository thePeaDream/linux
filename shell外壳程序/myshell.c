#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
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

//标志命令是否有重定向
int redir_status;
#define NOT_REDIR 0
#define INPUT_REDIR 1
#define OUTPUT_REDIR 2
#define APPEND_REDIR 3


//检查用户输入的命令是否有重定向，返回目标文件的起始地址，没有重定向返回NULL
char* checkRedirect(char* str)
{
    assert(str);
    while(*str != '\0' && *str !='>' && *str != '<')
    {
        ++str;
    }
    char* ret = NULL;
    if(*str != '\0')
    {
        if(*str == '<')
        {
            //输入重定向
            redir_status = INPUT_REDIR;
        }
        else if(*str == '>'&& *(str+1)!='>')
        {
            //输出重定向
            redir_status = OUTPUT_REDIR;
        }
        else if(*str == '>'&&*(str+1)=='>')
        {
            *str = '\0';
            str++;
            //追加重定向
            redir_status = APPEND_REDIR;
        }
        *str = '\0';
        //找到重定向的文件
        char* begin = str+1;
        //跳过空格
        while(*begin == ' ')
        {
            ++begin;
        }
        ret = begin;
    }
    else
    {
        redir_status = NOT_REDIR;
    }
    return ret;
}

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
        
        //ls > a.txt\0
        //判断命令是否存在重定向,返回要重定向的文件或NULL，无重定向返回空
        //要让子进程执行的程序进行重定向，所以要在fork()之后让子进程重定向文件
        char* redirect = checkRedirect(command);
        
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
             if(redirect!=NULL)
             {
                 const char* file = redirect;
                 int fd;
                 switch(redir_status)
                 {
                     case INPUT_REDIR:
                         fd = open(file,O_RDONLY);
                         dup2(fd,0);//本来要从键盘读取，现在从文件中读取
                         break;
                     case OUTPUT_REDIR:
                         fd = open(file,O_WRONLY|O_CREAT|O_TRUNC,0666);
                         dup2(fd,1);//本来要写到显示器的数据，现在写到对应文件中，将文件描述符表下标为1的内容 改成 下标为fd的内容，都指向新打开的文件
                         break;
                     case APPEND_REDIR:
                         fd = open(file,O_WRONLY|O_CREAT|O_APPEND,0666);
                         dup2(fd,1);
                         break;
                     default:
                         printf("重定向判断异常！！！\n");
                         break;
                 }
              }
             
            
            //子进程
            //printf("让子进程执行:%s\n",myargv[0]);
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
