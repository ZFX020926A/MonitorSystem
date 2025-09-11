#include "ThreadPool.hpp"

#include <unistd.h>

ThreadPool::ThreadPool(int threadNum, int taskNum)
: _threadNum(threadNum)
, _taskNum(taskNum)
, _isExit(false)
, _taskQueue(_taskNum)
, _threads()
{
    _threads.reserve(_threadNum);
}

ThreadPool::~ThreadPool()
{
}


void ThreadPool::ThreadPoolStart()
{
    for (int i = 0; i < _threadNum; ++i)
    {
        unique_ptr<Thread> upThread(new Thread(std::bind(&ThreadPool::doTask, this)));
        _threads.push_back(std::move(upThread));
        _threads[i]->ThreadStart();
    }
}

void ThreadPool::ThreadPoolStop()
{
    if(!_isExit)
    {
        while(!_taskQueue.empty())
        {
            sleep(1);
        }
        
        _isExit = true;
        _taskQueue.wakeup();
        for (int i = 0; i < _threadNum; ++i)
        {
            _threads[i]->ThreadStop();
        }
    }
}

void ThreadPool::addTask(Element &&ele)
{
    _taskQueue.push(std::move(ele));
}

void ThreadPool::doTask()
{
    while(!_isExit)
    {
        Element ele = _taskQueue.pop();
        if (ele)
        {
            ele();
        }
    }
}