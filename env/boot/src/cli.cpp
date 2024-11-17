#include "cli.hpp"

#include <stm32cpp/UART.hpp>

void CLI_Writer(const char *str)
{
    STM32::UART1_Driver::send((uint8_t *)str, strlen(str), nullptr);
}

// define handlers...
void CLI_HelpCommand()
{
    // TODO how to access commands list from here???
}

void CLI_Init()
{
    static V2::Command commands[] = {
        V2::Command{"?", CLI_HelpCommand}
    };

    CLI::configure(commands, sizeof(commands) / sizeof(V2::Command));
}
