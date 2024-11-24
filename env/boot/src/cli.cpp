#include "cli.hpp"

#include <stm32cpp/IO.hpp>

#include "uart.hpp"

namespace Console
{
    void Writer(uint8_t *buf, size_t len)
    {
        // STM32::IO::PC::disable();
        UART1::write(buf, len);//TODO WTF???
    }

    static void CLI_HelpCommand()
    {
        instance().write("HELLO");
    }

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

    static uint8_t STDIN[STDIN_BUFFER_SIZE] = {0};
    static uint8_t STDOUT[STDOUT_BUFFER_SIZE] = {0};

    static Command commands[] = {
        {"?", CLI_HelpCommand}};

    App<Writer> &instance()
    {
        static App<Writer> console{
            commands,
            sizeof(commands) / sizeof(Command),
            (char *)STDIN,
            STDIN_BUFFER_SIZE,
            (char *)STDOUT,
            STDOUT_BUFFER_SIZE};

        return console;
    }
}
