#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    //追加重定向
    int fd = open("log.txt",O_WRONLY|O_CREAT|O_APPEND,0777);
    if(fd < 0)
    {
        perror("open error");
        exit(1);
    }
    //将文件描述符表的array[fd]的内容拷贝给array[1],此时本来应该输出到屏幕上的内容，会输出到打开的文件中
    dup2(fd,1);
    fprintf(stdout,"abcdefg\n");
    const char* str = "abceee\n";
    fwrite(str,strlen(str),1,stdout);
    fflush(stdout);
    close(fd); 
    return 0;
}
