#include "TaskQueue.hpp"

TaskQueue::TaskQueue(int taskQueueSize)
: _taskQueueSize(taskQueueSize)
, _mutex()
, _notFull(_mutex)
, _notEmpty(_mutex)
, _flage(true)
{
}

TaskQueue::~TaskQueue()
{
}

bool TaskQueue::full()
{
    return _taskQueue.size() == static_cast<size_t>(_taskQueueSize);
}

bool TaskQueue::empty()
{
    return _taskQueue.size() == 0;;
}

void TaskQueue::push(Element &&ele)
{
    MutexLockGuard autolock(_mutex);
    while(full())
    {
        _notFull.wait();
    }

    _taskQueue.push(std::move(ele));
    _notEmpty.notify();
}

Element TaskQueue::pop()
{
    MutexLockGuard autolock(_mutex);
    while(empty() && _flage)
    {
        _notEmpty.wait();
    }

    if(!_flage)
    {
        return nullptr;
    }
    Element ele = _taskQueue.front();
    _taskQueue.pop();
    _notFull.notify();

    return ele;
}

void TaskQueue::wakeup()
{
    _flage = false;
    _notEmpty.notifyAll();
}

int TaskQueue::getTaskQueueSize()
{
    return _taskQueueSize;
}