#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#include <stdint.h>
#include <stm32cpp/Atomic.hpp>

typedef void (*TaskT)(void);

#ifndef DISPATCHER_MAX_TASKS
#define DISPATCHER_MAX_TASKS 10
#endif

namespace
{
    static TaskT tasks[DISPATCHER_MAX_TASKS];
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
    static bool pushTask(TaskT task)
    {
        if (_tasksLen >= DISPATCHER_MAX_TASKS)
        {
            return false;
        }

        DisableInterrupts di; //<-- wrap all below function body into atomic block

        tasks[_tasksTail] = task;
        _tasksTail++;

        if (_tasksTail >= DISPATCHER_MAX_TASKS)
            _tasksTail = 0;

        _tasksLen++;

        return true;
    }

    static void dispatch()
    {
        if (_tasksLen > 0)
        {
            TaskT &task = tasks[_tasksHead];
            ATOMIC
            {
                _tasksHead++;

                if (_tasksHead >= DISPATCHER_MAX_TASKS)
                    _tasksHead = 0;

                _tasksLen--;
            }
            task();
        }
    }
};

#endif // __DISPATCHER_H__