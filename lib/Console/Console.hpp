#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#include <algorithm>
#include <array>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>

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

    struct Arg
    {
        const char *name;
        const ArgType type;
        mutable ArgValue value;
    };

    typedef void (*CommandHandler)(void);

    struct Command
    {
        const char *name;
        const CommandHandler handler;
        const Arg *args_arr;
        const uint8_t args_num;
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
            for (size_t i = 0; i < _commands_num; i++)
            {
                if (0 == strcmp(_commands_arr[i].name, str))
                    return &_commands_arr[i];
            }
            return nullptr;
        }

        static inline bool parseArg(const char *str, ArgType type, ArgValue *val)
        {
            if (type == ArgType::INT)
            {
                int32_t result = atol(str);
                if (errno)
                    return false;
                val->int_value = result;
                return true;
            }
            else if (type == ArgType::STR)
            {
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
            uint8_t arg_index = 0;

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
                        else
                        {
                            if (arg_index == cmd->args_num)
                            {
                                write("ERR: too many args");
                                return;
                            }
                            const Arg *arg = &cmd->args_arr[arg_index];
                            ArgValue val;
                            if (!parseArg(buf, arg->type, &val))
                            {
                                write("ERR: invalid value for ");
                                write(arg->name);
                                return;
                            }
                            arg->value = val;
                            arg_index++;
                        }
                        token = nullptr;
                    }
                    else if (!token)
                    {
                        token = &buf[i]; // Set token addr if not set
                    }
                }
            }

            if (!cmd)
                return;

            cmd->handler();
        }
        static inline void write(const char *str)
        {
            writer(str);
        }
    };
}

namespace Console
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

    struct Arg
    {
        const char *name;
        const ArgType type;
        mutable ArgValue value;
    };

    typedef void (*CommandHandler)(void);

    struct Command
    {
        const char *name;
        const CommandHandler handler;
        const Arg *args_arr;
        const uint8_t args_num;
    };

    struct Buffer
    {
        char *data;
        size_t limit;
        size_t count{0};
    };

    class App
    {
    private:
        Command *m_aCommands;
        size_t m_nCommands;
        Buffer m_stdin;
        Buffer m_stdout;

        inline Command *find(const char *str)
        {
            for (size_t i = 0; i < m_nCommands; i++)
            {
                if (0 == strcmp(m_aCommands[i].name, str))
                {
                    return &m_aCommands[i];
                }
            }
            return nullptr;
        }

        inline bool parseArg(const char *str, ArgType type, ArgValue *val)
        {
            if (type == ArgType::INT)
            {
                int32_t result = atol(str);
                if (errno)
                    return false;
                val->int_value = result;
                return true;
            }
            else if (type == ArgType::STR)
            {
                val->str_value = str;
                return true;
            }
            return false;
        }

        inline void doProcess()
        {
            const Command *cmd = nullptr;
            const char *token = nullptr;
            uint8_t arg_index = 0;

            for (size_t i = 0; i < m_stdin.count; i++)
            {
                const char c = m_stdin.data[i];
                if (c == ' ' || c == '\0')
                {
                    if (!token)
                        return; // Empty line or extra whitespace - just skip handle

                    m_stdin.data[i] = '\0'; // Replace char to end terminator for proper parse arg
                    if (!cmd)
                    {
                        cmd = find(token);
                        if (!cmd)
                        {
                            write("ERR: command not found");
                            return;
                        }
                        else
                        {
                            if (arg_index == cmd->args_num)
                            {
                                write("ERR: too many args");
                                return;
                            }
                            const Arg *arg = &cmd->args_arr[arg_index];
                            ArgValue val;
                            if (!parseArg(m_stdin.data, arg->type, &val))
                            {
                                write("ERR: invalid value for ");
                                write(arg->name);
                                return;
                            }
                            arg->value = val;
                            arg_index++;
                        }
                        token = nullptr;
                    }
                    else if (!token)
                    {
                        token = &m_stdin.data[i]; // Set token addr if not set
                    }
                }
            }

            if (!cmd)
                return;

            cmd->handler();
        }

    public:
        App() = delete;
        App(
            Command *aCommands,
            size_t nCommands,
            char *stdinBuf,
            size_t stdinMax,
            char *stdoutBuf,
            size_t stdoutMax)
            : m_aCommands(aCommands),
              m_nCommands(nCommands),
              m_stdin{stdinBuf, stdinMax},
              m_stdout{stdoutBuf, stdoutMax}
        {
        }

    public:
        bool process(const char *buf, size_t len)
        {
            if (len > m_stdin.limit)
            {
                return false;
            }

            m_stdin.count = len;
            memcpy(m_stdin.data, buf, len);

            doProcess();
            return true;
        }

        void write(const char *)
        {
            //TODO use buffer for write, maybe per char loop for trigger flush on \n char
            //TODO need writer class with write & isReady methods, for prevent collisions
        }
    };
}

#endif // __CONSOLE_HPP__