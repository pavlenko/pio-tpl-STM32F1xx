#ifndef __CONSOLE_V3_H__
#define __CONSOLE_V3_H__

#include <cstdint>

namespace V3
{
    enum class ArgType
    {
        INT,
        STR
    };

    struct Arg
    {
        const char *name;
        ArgType type;
    };

    typedef void (*CommandHandler)();

    struct Command
    {
        const char *name;
        Arg *args;
        uint8_t argc;
    };
    // argT, arg, command, console
}

#endif // __CONSOLE_V3_H__