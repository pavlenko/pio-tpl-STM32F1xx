#ifndef __DISPATCHER_HPP__
#define __DISPATCHER_HPP__

#include <stddef.h>
#include <stdint.h>

#include <AtomicBlock.hpp>

namespace Dispatcher
{
    typedef void (*TaskFunctionT)();
    typedef void (*TaskCallbackT)(void *argument);

    class Task
    {
        TaskCallbackT m_callback;
        void *m_argument;

    public:
        Task(TaskCallbackT callback = nullptr, void *argument = nullptr)
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

        template <class ObjectT, void (ObjectT::*method)()>
        static void invokeMethod(void *object) { (static_cast<ObjectT *>(object)->*method)(); }

        static void invokeFunction(void *function) { reinterpret_cast<TaskFunctionT>(function)(); }

        template <class FunctorT>
        static void invokeFunctor(void *functor) { (*reinterpret_cast<FunctorT *>(functor))(); }

    public:
        Dispatcher(Task *tasks, size_t tasksMax)
            : m_tasks(tasks), m_tasksMax(tasksMax)
        {
        }

        template <class FunctorT>
        bool pushTask(FunctorT &functor) { return pushTask(&invokeFunctor<FunctorT>, (void *)&functor); }

        template <class FunctorT>
        bool pushTask(const FunctorT &functor) { return pushTask(&invokeFunctor<FunctorT>, (void *)&functor); }

        template <class ObjectT, void (ObjectT::*method)()>
        bool pushTask(ObjectT *object) { return pushTask(&invokeMethod<ObjectT, method>, object); }

        bool pushTask(TaskFunctionT function) { return pushTask(invokeFunction, (void *)function); }

        inline bool pushTask(TaskCallbackT callback, void *argument);

        inline void dispatch();
    };

    bool Dispatcher::pushTask(TaskCallbackT callback, void *argument)
        {
            if (m_tasksNum >= m_tasksMax)
                return false;

            Task task(callback, argument);
            AtomicBlock atomic;

            m_tasks[m_tasksTail] = task;
            m_tasksTail++;

            if (m_tasksTail >= m_tasksMax)
                m_tasksTail = 0;

            m_tasksNum++;

            return true;
        }

    void Dispatcher::dispatch()
    {
        if (m_tasksNum > 0)
        {
            Task &task = m_tasks[m_tasksHead];
            {
                AtomicBlock atomic;
                m_tasksHead++;

                if (m_tasksHead >= m_tasksMax)
                    m_tasksHead = 0;

                m_tasksNum--;
            }
            task.invoke();
        }
    }

    Dispatcher &instance();
}

#endif // __DISPATCHER_HPP__