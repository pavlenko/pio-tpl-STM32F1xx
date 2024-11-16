#include "cli.hpp"

#include <stm32cpp/UART.hpp>

void CLI_Writer(const char *str)
{
    STM32::UART1_Driver::send((uint8_t *)str, strlen(str), nullptr);
}

//define handlers...
void CLI_HelpCommand(){}

void CLI_Init()
{
    //TODO somehow add commands, maybe use prev/next refs for dynamic allocation...
    // Command{Command *prev, Command *next}...
    // const V2::Command Help{"?"};

    const V2::Command commands[] = {V2::Command{"?", CLI_HelpCommand}};

    CLI::configure(commands);
}