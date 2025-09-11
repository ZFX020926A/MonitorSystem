#include "MutexLock.hpp"

MutexLock::MutexLock()
{
    // 初始化互斥锁
    pthread_mutex_init(&_mutex, nullptr);
}

MutexLock::~MutexLock()
{
    // 销毁互斥锁
    pthread_mutex_destroy(&_mutex);
}

void MutexLock::lock()
{
    pthread_mutex_lock(&_mutex);
}

void MutexLock::unlock()
{
    pthread_mutex_unlock(&_mutex);
}

pthread_mutex_t * MutexLock::getMutexPtr()
{
    return &_mutex;
}