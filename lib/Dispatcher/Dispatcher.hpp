#ifndef __DISPATCHER_HPP__
#define __DISPATCHER_HPP__

#include <stdint.h>

#include <AtomicBlock.hpp>

#ifndef DISPATCHER_MAX_TASKS
#define DISPATCHER_MAX_TASKS 10
#endif

namespace
{
    typedef void (*TaskHandler)(void);

    struct Task
    {
        TaskHandler handler;
        Task(TaskHandler handler = nullptr) : handler(handler) {}
        inline void invoke()
        {
            if (handler)
                handler();
        }
    };

    static Task _tasks[DISPATCHER_MAX_TASKS];
    static uint8_t _tasksLen;
    static uint8_t _tasksHead;
    static uint8_t _tasksTail;
}

class Dispatcher
{
    Dispatcher(const Dispatcher &) = delete;     // Prevent copy constructor
    Dispatcher(Dispatcher &&) = delete;          // Prevent move constructor
    void operator=(const Dispatcher &) = delete; // Prevent assignment
    void operator=(Dispatcher &&) = delete;      // Prevent reference

public:
    static inline bool pushTask(TaskHandler &handler)
    {
        if (_tasksLen >= DISPATCHER_MAX_TASKS)
        {
            return false;
        }

        Task task(handler);
        AtomicBlock atomic;

        _tasks[_tasksTail] = task;
        _tasksTail++;

        if (_tasksTail >= DISPATCHER_MAX_TASKS)
            _tasksTail = 0;

        _tasksLen++;

        return true;
    }

    static inline void dispatch()
    {
        if (_tasksLen > 0)
        {
            Task &task = _tasks[_tasksHead];
            {
                AtomicBlock atomic;
                _tasksHead++;

                if (_tasksHead >= DISPATCHER_MAX_TASKS)
                    _tasksHead = 0;

                _tasksLen--;
            }
            task.invoke();
        }
    }
};

#endif // __DISPATCHER_HPP__