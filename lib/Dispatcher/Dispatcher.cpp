#include <Dispatcher.hpp>

namespace Dispatcher
{
#if defined(DISPATCHER_MAX_TASKS)
    static const int tasksMax = DISPATCHER_MAX_TASKS;
#else
    static const int tasksMax = 10;
#endif

    static Task tasks[tasksMax];
    static Dispatcher dispatcher{tasks, tasksMax};

    Dispatcher &instance()
    {
        return dispatcher;
    }
}
