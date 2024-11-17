#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>

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

namespace V2
{
    enum class ArgType
    {
        INT,
        STR,
    };

    union ArgValue
    {
        intptr_t int_value;
        const char *str_value;
        void *any_value;
    };

    typedef void (*CommandHandler)(void);

    struct Command
    {
        const char *name;
        const CommandHandler handler;
    };

    typedef void (*ConsoleWriter)(const char *);

    template <ConsoleWriter tWriter>
    class Console
    {
        static constexpr const ConsoleWriter writer = tWriter;

        static inline Command *_commands_arr;
        static inline uint8_t _commands_num;

        static inline Command *find(const char *str)
        {
            for (size_t i; i < _commands_num; i++)
            {
                if (0 == strcmp(_commands_arr[i].name, str))
                    return &_commands_arr[i];
            }
            return nullptr;
        }

        static inline bool parseArg(const char *str, ArgType type, ArgValue *val)
        {
            switch (type)
            {
            case ArgType::INT:
                int32_t result = atol(str);
                if (errno)
                    return false;
                val->int_value = result;
                return true;
            case ArgType::STR:
                val->str_value = str;
                return true;
            }
            return false;
        }

    public:
        static inline void configure(Command *arr, size_t num)
        {
            _commands_arr = arr;
            _commands_num = num;
        }

        static inline void process(char *buf, size_t len)
        {
            const Command *cmd = nullptr;
            const char *token = nullptr;

            for (size_t i = 0; i < len; i++)
            {
                const char c = buf[i];
                if (c == ' ' || c == '\0')
                {
                    if (!token)
                        return; // Empty line or extra whitespace - just skip handle

                    buf[i] = '\0'; // Replace char to end terminator for proper parse arg
                    if (!cmd)
                    {
                        cmd = find(token);
                        if (!cmd)
                        {
                            write("ERR: command not found");
                            return;
                        }
                        //TODO args
                    }
                    else if (!token)
                    {
                        token = &buf[i]; // Set token addr if not set
                    }
                }
            }
        }
        static inline void write(const char *str)
        {
            writer(str);
        }
    };
}

#endif // __CONSOLE_HPP__