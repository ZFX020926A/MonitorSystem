#include "Thread.hpp"

Thread::Thread(ThreadCallBack &&cb)
: _pThreadID(0)
, _isRunning(false)
, _cb(std::move(cb))
{
}

Thread::~Thread()
{
}

void Thread::ThreadStart()
{
    int ret = pthread_create(&_pThreadID, nullptr, threadIntoFunc, this);
    if(ret != 0)
    {
        // 创建线程失败
        return; 
    }

    // 线程创建成功 开始运行
    _isRunning = true;
}

void Thread::ThreadStop()
{
    if(_isRunning)
    {
        pthread_join(_pThreadID, nullptr);
        _isRunning = false;
    }
}

void * Thread::threadIntoFunc(void *arg)
{
    Thread *pThread = static_cast<Thread *>(arg);
    if(pThread)
    {
        pThread->_cb();
    }
    return nullptr;
}




