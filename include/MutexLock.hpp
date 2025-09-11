#ifndef __MUTEXLOCK_H__
#define __MUTEXLOCK_H__

// 此类用来封装互斥锁的操作 和 一个自动锁

#include <pthread.h>
#include <mutex>    

class MutexLock
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();

    pthread_mutex_t * getMutexPtr();
private:
    pthread_mutex_t _mutex;
};

class MutexLockGuard
{

public:
    MutexLockGuard(MutexLock & mutexLock)
    : _mutexLock(mutexLock)
    {
        _mutexLock.lock();
    }

    ~MutexLockGuard()
    {
        _mutexLock.unlock();
    }

private:
    MutexLock & _mutexLock;
};


#endif // !__MUTEXLOCK_H__