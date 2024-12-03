#pragma once

#include <stm32cpp/DMA.hpp>
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
    private:
        static constexpr USART_TypeDef *regs() { return reinterpret_cast<USART_TypeDef *>(RegsT); }

    public:
        // enable UART, enable IRQ vector
        static void enable();
        // disable UART, disable IRQ vector
        static void disable();
        // configure bus
        static void configure(Config config);
        // send data
        static void send(uint8_t *data, uint16_t size);
        static void sendDMA(uint8_t *data, uint16_t size)
        {
            while (busyTX())
                asm("nop");

            DMA1_Channel1::clrFlag<DMA::Flag::TRANSFER_COMPLETE>();
            // DMA1_Channel1::setTransferCallback();//<-- need internal callback

            regs()->CR1 |= USART_CR3_DMAT;

            DMA1_Channel1::transfer<DMA::Config::MEM_2_PER | DMA::Config::MINC>(data, &regs()->DR, size);
        }
        // recv data
        static void recv(uint8_t *data, uint16_t size);
        // check ready tx
        static bool busyTX() { return false; }
        // check ready rx
        static bool busyRX() { return false; }
        static void dispatchIRQ();
    };
}
