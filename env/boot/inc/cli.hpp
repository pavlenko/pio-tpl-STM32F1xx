#ifndef __CLI_HPP__
#define __CLI_HPP__

#include <Console.hpp>

namespace Console
{
#if defined(CONSOLE_STDIN_BUFFER_SIZE)
    static const size_t STDIN_BUFFER_SIZE = CONSOLE_STDIN_BUFFER_SIZE;
#else
    static const size_t STDIN_BUFFER_SIZE = 64;
#endif

#if defined(CONSOLE_STDOUT_BUFFER_SIZE)
    static const size_t STDOUT_BUFFER_SIZE = CONSOLE_STDOUT_BUFFER_SIZE;
#else
    static const size_t STDOUT_BUFFER_SIZE = 64;
#endif

    App &instance();
}

void CLI_Writer(const char *str);

using CLI = V2::Console<CLI_Writer>;

void CLI_Init();

#endif // __CLI_HPP__