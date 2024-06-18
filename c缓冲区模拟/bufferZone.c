#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
//缓冲区大小
#define BUFFER_SIZE 1024

typedef struct myFILE
{
    int fd;
    char buffer[BUFFER_SIZE];
    int end; //当前缓冲区的结尾
}myFILE;
    
myFILE* myFopen(const char* str,const char* mode)
{
    assert(str);
    assert(mode);
    myFILE* fp = NULL;
    if(strcmp(mode,"w") == 0)
    {
        int fd = open(str,O_WRONLY|O_CREAT|O_TRUNC,0666);
        if(fd >= 0)
        {
            fp = (myFILE*)calloc(1,sizeof(myFILE));
            fp->fd = fd;
        }
    }
    return fp;
}

void myFputs(const char* str, myFILE* fp)
{
   //写入到缓冲区中
   strcpy(fp->buffer+fp->end,str);
   fp->end += strlen(str);
   int end = fp->end -1;
   if(fp->fd == 1)//标准输出  行刷新/行缓冲
   {
        for(;end >= 0 && fp->buffer[end] != '\n';--end)
        {
            //从后往前找到第一个'\n'的位置，将'\n'之前的内容全部刷新到内核
        }
        if(end >= 0)
        {
            //说明找到了'\n'
            //将包含'\n'之前的数据刷新到内核
            write(fp->fd,fp->buffer,end+1);//fp->buffer[end] = '\n' \n也要刷出去
            //'\n'后面的数据往前移动
            int move = end + 1;
            int count = 0;//记录'\n'后面的数据个数
            while(fp->buffer[move] != '\0')
            {
                fp->buffer[count++] = fp->buffer[move++];
            }
            fp->buffer[count] = '\0';
            //更新fp
            fp->end = count;
        }
   }
}


void myFFlush(myFILE* fp)
{
   //调用write接口写入到内核
   if(fp->end > 0)
   {
        write(fp->fd,fp->buffer,fp->end);
        fp->end = 0;
        fp->buffer[0] = '\0';
   }
}

void myClose(myFILE* fp)
{
    //关闭文件之前先刷新缓冲区
    myFFlush(fp);
    //关闭fd
    close(fp->fd);
    //释放myFILE的空间
    free(fp);
}

int main()
{
    //close(1); 
    //在进程工作路径下打开log.txt
    myFILE* fp = myFopen("log.txt","w");
    myFputs("abcdefg\n",fp);
    //缓冲区的数据没有被刷新，fork之后也会有两份缓冲区数据，导致写到文件里有2个abcdefg
    fork();
    myClose(fp);
    return 0;
}
