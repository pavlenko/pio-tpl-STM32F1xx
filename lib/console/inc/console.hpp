#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#include <algorithm>
#include <array>
#include <cstdint>

// struct for allow pass string as template parameter & use at compile time
// template <std::size_t N>
// struct String
// {
//     char data[N];
//     consteval String(const char (&str)[N])
//     {
//         std::copy_n(str, N, data);
//     }
//     // TODO operators
// };

// typedef void (*Handler_)(void);

// enum class ArgType_
// {
//     INT,
//     STR,
// };

// template <String tName, ArgType_ tType>
// struct Arg_
// {
//     static constexpr const char *name = tName.data;
//     static constexpr const ArgType_ type = tType;
//     // TODO value here...
// };

// template <String tName, Handler_ tHandler, class... tArgs>
// struct Command_
// {
//     static constexpr const char *name = tName.data;
//     static constexpr const Handler_ handler = tHandler;
//     static constexpr const uint8_t argc = sizeof...(tArgs);

//     // static constexpr const auto args[argc] = {tArgs()...};
// };

namespace V2
{
    // Command
    struct Command
    {
        const char *name;
    };

    typedef void (*ConsoleWriter)(const char *);

    template <ConsoleWriter tWriter, class ... tCommands>
    class Console
    {
        static constexpr const ConsoleWriter writer = tWriter;
        static constexpr const std::array<Command, sizeof...(tCommands)> commands{tCommands()...};//<--???

    public:
        static inline void process(char *buf, size_t len)
        {
            //
        }
        static inline void write(const char *str)
        {
            writer(str);
        }
    };
}

#endif // __CONSOLE_HPP__