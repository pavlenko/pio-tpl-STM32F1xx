#include "cli.hpp"

#include <stm32cpp/UART.hpp>

void CLI_Writer(const char *str)
{
    STM32::UART1_Driver::send((uint8_t *)str, strlen(str), nullptr);
}

void CLI_Init()
{
    // Console::write("Initialized");
}