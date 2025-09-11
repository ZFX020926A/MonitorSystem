#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

// 此文件用于封装线程池
#include "TaskQueue.hpp"
#include "Thread.hpp"

#include <vector>
#include <memory>

using std::vector;
using std::unique_ptr;

class ThreadPool
{
public:
    ThreadPool(int threadNum, int taskNum);
    ~ThreadPool();

    void ThreadPoolStart();
    void ThreadPoolStop();
    void addTask(Element &&ele);

private:
    void doTask();

private:
    int _threadNum;
    int _taskNum;
    bool _isExit;
    TaskQueue _taskQueue;
    vector<unique_ptr<Thread>> _threads;
};


#endif // !__THREADPOOL_H__