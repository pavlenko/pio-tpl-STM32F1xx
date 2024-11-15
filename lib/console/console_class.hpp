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
    // TODO operators
};

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
    // TODO value here...
};

template <String tName, Handler_ tHandler, class... tArgs>
struct Command_
{
    static constexpr const char *name = tName.data;
    static constexpr const Handler_ handler = tHandler;
    static constexpr const uint8_t argc = sizeof...(tArgs);

    // static constexpr const auto args[argc] = {tArgs()...};
};

namespace V2
{
    // Command
    //...

    typedef void (*ConsoleWriter)(const char *);

    //TODO pass commands somehow at compile time...
    template <ConsoleWriter tWriter, typename... tCommands>
    class Console
    {
        static constexpr const ConsoleWriter writer = tWriter;

    public:
        static inline void write(const char *str)
        {
            writer(str);
        }
    };
}

#endif // __CONSOLE_CLASS_H__