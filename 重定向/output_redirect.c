#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main()
{
    //本来应该打印到显示器的内容，写到了文件中 ——输出重定向
    close(1);
    int fd = open("log.txt",O_CREAT|O_WRONLY|O_TRUNC);
    if(fd < 0)
    {
        perror("open:");
        exit(1);
    }
    printf("fd:%d\n",fd);
    fprintf(stdout,"fd:%d\n",fd);
    const char* str = "abcdefg\n";
    fwrite(str,strlen(str),1,stdout);
    fflush(stdout);
    close(fd);
    return 0;
}
