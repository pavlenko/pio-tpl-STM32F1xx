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
        // enable UART, enable IRQ vector
        static void enable();
        // disable UART, disable IRQ vector
        static void disable();
        // configure bus
        static void configure(Config config);
        // send data
        static void send(uint8_t *data, uint16_t size);
        // recv data
        static void recv(uint8_t *data, uint16_t size);
        // check ready tx
        static bool busyTX();
        // check ready rx
        static bool busyRX();
        static void dispatchIRQ();
    };
}
