#pragma once
#include <iostream>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define TASK_VOLUME 3
using namespace std;
typedef std::function<void()> func; 
unordered_map<int,string> taskDesc;
vector<func> taskContainer;
void readMySQL()
{
    std::cout << "读取MySQL" << std::endl;
}
void cal()
{
    std::cout << "加密计算"<<std::endl;
}
void save()
{
    std::cout << "保存"<<std::endl;
}

//导入所有任务到任务容器中
void loadTask()
{
    taskContainer.push_back(readMySQL);
    taskDesc.insert({0,"readMySQL:读取数据库"});

    taskContainer.push_back(cal);
    taskDesc.insert({1,"cal:计算"});
    
    taskContainer.push_back(save);
    taskDesc.insert({2,"save:保存"});
}
//展示所有任务
void showTask()
{
    for(const pair<int,string>& task:taskDesc)
        cout << task.first << '\t' << task.second << endl;
}
//分配任务给子进程
void assignTask(pid_t childId,int writefd,int taskNumber)
{
    write(writefd,&taskNumber,sizeof(taskNumber));
    cout << "成功分配任务给子进程: " << childId << endl;
}