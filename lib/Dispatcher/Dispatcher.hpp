#ifndef __DISPATCHER_HPP__
#define __DISPATCHER_HPP__

#include <stdint.h>

#include <AtomicBlock.hpp>

// TODO make no static ???
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
    static inline bool pushTask(TaskHandler handler)
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

namespace Dispatcher_
{
    typedef void (*Callback)(void *argument);

    class Task
    {
        Callback m_callback;
        void *m_argument;

    public:
        Task(Callback callback = nullptr, void *argument = nullptr)
            : m_callback(callback), m_argument(argument)
        {
        }
        void invoke()
        {
            if (m_callback)
                m_callback(m_argument);
        }
    };

    class Dispatcher
    {
    private:
        Task *m_tasks;
        size_t m_tasksMax;
        size_t m_tasksNum{0};
        size_t m_tasksHead{0};
        size_t m_tasksTail{0};

        Dispatcher(const Dispatcher &) = delete;
        Dispatcher &operator=(const Dispatcher &) = delete;

    public:
        Dispatcher(Task *tasks, size_t tasksMax)
            : m_tasks(tasks), m_tasksMax(tasksMax)
        {
        }

        template <class Functor>
        bool pushTask(Functor &functor);

        template <class Functor>
        bool pushTask(const Functor &functor);

        template <class Object, void (Object::*method)()>
        bool pushTask(Object *object);

        bool pushTask(void (*function)());

        bool pushTask(Callback callback, void *argument)
        {
            if (m_tasksNum >= DISPATCHER_MAX_TASKS)
                return false;

            Task task(callback, argument);
            AtomicBlock atomic;

            m_tasks[m_tasksTail] = task;
            m_tasksTail++;

            if (m_tasksTail >= DISPATCHER_MAX_TASKS)
                m_tasksTail = 0;

            m_tasksNum++;

            return true;
        }

        void dispatch()
        {
            if (m_tasksNum > 0)
            {
                Task &task = m_tasks[m_tasksHead];
                {
                    AtomicBlock atomic;
                    m_tasksHead++;

                    if (m_tasksHead >= DISPATCHER_MAX_TASKS)
                        m_tasksHead = 0;

                    m_tasksNum--;
                }
                task.invoke();
            }
        }
    };

    Dispatcher &instance();
}

#endif // __DISPATCHER_HPP__