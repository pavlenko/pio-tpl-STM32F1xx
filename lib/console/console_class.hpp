#ifndef __CONSOLE_CLASS_H__
#define __CONSOLE_CLASS_H__

#include <algorithm>

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

typedef void (*write_fn)(const char *);

typedef void (*Handler_)(void);

enum class ArgType_
{
    INT,
    STR,
};

template <String tName, ArgType_ tType>
struct Arg_
{
    static constexpr const char *name = tName.data;
    static constexpr const ArgType_ type = tType;
    //TODO value here...
};

template <typename... tArgs>//TODO avoid
struct ArgList_
{
    static constexpr const uint8_t num = sizeof...(tArgs);
};

template <String tName, Handler_ tHandler, class tArgs>//TODO last variadic
struct Command_
{
    static constexpr const char *name = tName.data;
    static constexpr const Handler_ handler = tHandler;
};

template <write_fn tWriter, typename... tCommands>
class Console_
{
    static constexpr const write_fn writeFn = tWriter;
};

#endif // __CONSOLE_CLASS_H__