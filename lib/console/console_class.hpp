#ifndef __CONSOLE_CLASS_H__
#define __CONSOLE_CLASS_H__

//pass writeFn as tpl arg is possible

#include <algorithm>

//TODO other parts of cli types

// struct for allow pass string as template parameter & use at compile time
template <std::size_t N>
struct String
{
    char data[N];
    consteval String(const char (&str)[N])
    {
        std::copy_n(str, N, data);
    }
};

typedef void(*write_fn)(const char *);

template <String tName>
struct Command_
{
    static constexpr String name = tName;
};

template <write_fn tWriter, typename ...tCommands>
class Console_
{
    //
};

#endif // __CONSOLE_CLASS_H__