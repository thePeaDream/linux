//客户端发送数据
#include "com.hpp"
using namespace std;
int main()
{
    //打开管道文件
    int fd = open(pipe_path.c_str(),O_WRONLY);
    if(fd < 0)
    {
        perror("open");
        exit(1);
    }

    string buffer;
    while(true)
    {
        cout << "please client sent something Line > ";
        getline(cin,buffer);
        ssize_t s = write(fd,buffer.c_str(),sizeof buffer);
    }
    close(fd);
    return 0;
}