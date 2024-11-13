#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef CONSOLE_MAX_CMD_NUM
#define CONSOLE_MAX_CMD_NUM 2
#endif

#ifndef CONSOLE_MAX_CMD_LEN
#define CONSOLE_MAX_CMD_LEN 16
#endif

#ifndef CONSOLE_MAX_CMD_ARGS
#define CONSOLE_MAX_CMD_ARGS 0
#endif

namespace Console
{
    enum class ArgumentType
    {
        INT,
        STR
    };

    // TODO redefine types & maybe add pointer type helpers...
    typedef void (*Handler_t)(void);

    typedef union
    {
        intptr_t int_value;
        const char *str_value;
        void *any_value;
    } ArgValue_t;

    typedef struct
    {
        const char *name;
        ArgumentType type;
    } Arg_t;

    typedef struct
    {
        const char *name;
        Handler_t handler;
        const Arg_t *args;
        uint8_t argc;
        void **argv;
    } Command_t;

    // union ArgumentValue
    // {
    //     intptr_t int_value;
    //     const char *str_value;
    //     void *any_value;
    // };

    // struct Argument
    // {
    //     ArgumentType type;
    //     ArgumentValue value;
    // };

    // typedef void (*Handler)(void);

    // struct Command
    // {
    //     const char *name;
    //     Handler handler;
    //     Argument *args;
    //     uint8_t argc;

    //     // Required default constructor
    //     Command() : name(""), handler(nullptr), args(nullptr), argc(0)
    //     {
    //     }
    //     Command(const char *name, Handler handler, Argument *args, uint8_t argc)
    //         : name(name), handler(handler), args(args), argc(argc)
    //     {
    //     }
    // };

    class Application
    {
    private:
        static inline void (*_writeFn)(const char *str);
        static inline Command_t *_commands_arr[CONSOLE_MAX_CMD_NUM];
        static inline uint8_t _commands_num;

        static inline const Command_t *find(const char *name)
        {
            for (uint8_t i = 0; i < _commands_num; i++)
            {
                if (0 == strcmp(_commands_arr[i]->name, name))
                {
                    return _commands_arr[i];
                }
            }
            return nullptr;
        }
        static inline bool parseArg(const char *str, const Arg_t *arg, ArgValue_t *val)
        {
            if (arg->type == ArgumentType::INT)
            {
                char *end = nullptr;
                long int result = strtol(str, &end, 0);
                if (result == LONG_MAX || result == LONG_MIN || end == nullptr || *end != '\0')
                    return false;

                val->int_value = (intptr_t)result;
                return true;
            }
            if (arg->type == ArgumentType::STR)
            {
                val->str_value = str;
                return true;
            }
            return false;
        }

    public:
        static inline void setWriteFn(void (*fn)(const char *str))
        {
            _writeFn = fn;
        }
        static inline void add(Command_t *cmd)
        {
            if (_commands_num == CONSOLE_MAX_CMD_NUM)
                return;

            _commands_arr[_commands_num++] = cmd;
        }
        static inline void process(char *buf, uint16_t len)
        {
            const Command_t *cmd = nullptr;
            uint32_t arg_index = 0;
            const char *token = nullptr;

            for (uint16_t i = 0; i < len; i++)
            {
                const char c = buf[i];
                if (c == ' ' || c == '\0')
                {
                    if (!token)
                        return; // Empty line or extra whitespace - just skip handle

                    buf[i] = '\0'; // Replace char to end terminator but why???
                    if (!cmd)
                    {
                        cmd = find(token);
                        if (!cmd)
                        {
                            writeln("ERR: command not found");
                            return;
                        }
                    }
                    else
                    {
                        if (arg_index == cmd->argc)
                        {
                            writeln("ERR: too many args");
                            return;
                        }
                        ArgValue_t val;
                        if (!parseArg(token, &cmd->args[arg_index], &val))
                        {
                            writeln("ERR: inavlid arg value");
                            return;
                        }
                        cmd->argv[arg_index] = &val.any_value;
                        arg_index++;
                    }
                    token = nullptr; // Clr token addr
                }
                else if (!token)
                {
                    token = &buf[i]; // Set token addr if not set
                }
            }

            if (!cmd)
                return;

            cmd->handler();
        }
        static inline void writeln(const char *str)
        {
            if (_writeFn)
                _writeFn(str);
        }
    };
}

#endif // __CONSOLE_HPP__
