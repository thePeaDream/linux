#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//这里的fd分配规则是：从头遍历文件描述符表，第一个没有被占用的文件描述符，给对应进程新打开的文件
int main()
{
    //输入重定向，本来应该从键盘里读取，现在从文件中读取
    close(0);
    int fd = open("log.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("open error");
        exit(1);
    }
    char buffer[64];
    memset(buffer,'\0',sizeof buffer);
    //stdin 内部封装了 fd:0,此时文件描述符表的下标为0的内容，不再指向键盘，而是新打开的文件
    while(fgets(buffer,sizeof buffer,stdin) != NULL)//按行读取
    {
        fprintf(stdout,"%s",buffer);
    }
    close(fd);
    return 0;
}
