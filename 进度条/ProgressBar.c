#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define LOAD 101
int main()
{
    //百分比进度条
    int percent = 0;
    char bar[LOAD];
    memset(bar,0,sizeof(bar));
    
    while(percent <= 100)
    {
        //每打印一次 回车覆盖上一次打印结果
        printf("[%-100s][%d%%]\r",bar,percent);
        bar[percent++] = '#';
        fflush(stdout);
        usleep(50000);//50毫秒
    }
    printf("\n");
    return 0;
}
