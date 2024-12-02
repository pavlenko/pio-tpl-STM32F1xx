#pragma once

#include <stm32cpp/UART_definitions.hpp>

extern "C"
{
#include <stm32f1xx_hal_uart.h>
}

using namespace STM32::UART;

namespace STM32::UARTex
{
    struct Config
    {
    };

    template <uint32_t RegsT, IRQn_Type IRQnT, class ClockT, class DMAtxT, class DMArxT>
    class Driver
    {
        static void enable();
        static void disable();
        static void configure(Config config);
        static void send(uint8_t *data, uint16_t size);
        static void recv(uint8_t *data, uint16_t size);
        static bool busyTX();
        static bool busyRX();
        static void dispatchIRQ();
    };
}
