#include "cli.hpp"

#include <stm32cpp/UART.hpp>

void CLI_Writer(const char *str)
{
    // TODO make buffered output, if received \n char then we can flush buffer
    // TODO on flush - copy data to tx buffer, clear cli buffer, then send tx buffer...
    STM32::UART1_Driver::send((uint8_t *)str, strlen(str), nullptr);
}

// define handlers...
void CLI_HelpCommand()
{
    // TODO how to access commands list from here???
}

void CLI_Init()
{
    // TODO in/out buffers, also allow read/write them outside of console
    // TODO move commands array to global namespace or annonymous for allow access from help command
    static V2::Command commands[] = {
        V2::Command{"?", CLI_HelpCommand}
    };

    CLI::configure(commands, sizeof(commands) / sizeof(V2::Command));
}

namespace Console
{
    static uint8_t STDIN[STDIN_BUFFER_SIZE];
    static uint8_t STDOUT[STDOUT_BUFFER_SIZE];
    // TODO commands definitions here
}

Console::App &ConsoleApp()
{
    using namespace Console;
    static Console::App console{nullptr, 0, STDIN, STDIN_BUFFER_SIZE, STDOUT, STDOUT_BUFFER_SIZE};
    return console;
}
