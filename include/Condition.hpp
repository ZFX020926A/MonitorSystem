#ifndef __CONDITION_H__
#define __CONDITION_H__

// 此文件用于封装条件变量

#include "MutexLock.hpp"

#include <pthread.h>

class Condition
{

public:
    Condition(MutexLock &mutex);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();

private:
    pthread_cond_t _cond;
    MutexLock &_mutex;
};




#endif // !__CONDITION_H__