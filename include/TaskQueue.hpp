#ifndef __TASKQUEUE_H__
#define __TASKQUEUE_H__

// 任务队列 的封装

#include "MutexLock.hpp"
#include "Condition.hpp"
#include "Task.hpp"

#include <queue>
#include <functional>

using std::queue;
using std::function;

using Element = Task;

class TaskQueue
{
public:
    TaskQueue(int taskQueueSize);
    ~TaskQueue();

    bool full();
    bool empty();
    void push(Element &&ele);
    Element pop();
    void wakeup();
    int getTaskQueueSize();
private:
    int _taskQueueSize;
    MutexLock _mutex;
    Condition _notFull;
    Condition _notEmpty;
    queue<Element> _taskQueue;
    bool _flage;
};




#endif // !__TASKQUEUE_H__
