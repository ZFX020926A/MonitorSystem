#ifndef __THREAD_H__
#define __THREAD_H__

// 此文件用于封装线程类 单个线程的操作

#include <pthread.h>
#include <functional>

using std::function;

using ThreadCallBack = function<void()>;

class Thread
{
public:
    Thread(ThreadCallBack &&cb);
    
    ~Thread();

    void ThreadStart();
    void ThreadStop();

private:
    static void *threadIntoFunc(void *arg); // 线程入口函数
private:
    pthread_t _pThreadID;
    bool _isRunning;
    ThreadCallBack _cb; // 线程回调函数
};






#endif // !__THREAD_H__
