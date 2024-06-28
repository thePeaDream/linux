#ifndef LOG_HPP_
#define LOG_HPP_
//日志信息打印
#include <iostream>
#include <ctime>
#include <string>
#define DEBUG 0
#define NOTICE 1
#define WARNING 2
#define ERROR 3
using namespace std;

std::ostream &log(const string& message,int level)
{
    const string msg[] = {
        "DEBUG",
        "NOTICE",
        "WARNING",
        "ERROR"
    };
    cout << "|" << (unsigned)time(nullptr)<< "|" << msg[level]<<"|" << message;
    return std::cout;
}
#endif
