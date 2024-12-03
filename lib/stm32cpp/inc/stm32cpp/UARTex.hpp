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

#define UART_TPL_ARGUMENTS template <uint32_t RegsT, IRQn_Type IRQnT, class ClockT, class DMAtxT, class DMArxT>
#define UART_TPL_QUALIFIER Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>

    UART_TPL_ARGUMENTS
    class Driver
    {
    private:
        static inline State txState;
        static inline State rxState;
        static constexpr USART_TypeDef *regs() { return reinterpret_cast<USART_TypeDef *>(RegsT); }

    public:
        // enable UART, enable IRQ vector
        static void enable();
        // disable UART, disable IRQ vector
        static void disable();
        // configure bus
        static void configure(Config config);
        static inline void sendIRQ(uint8_t *data, uint16_t size);
        static inline void recvIRQ(uint8_t *data, uint16_t size);
        static inline void sendDMA(uint8_t *data, uint16_t size);
        static inline void recvDMA(uint8_t *data, uint16_t size);
        static inline bool busyTX();
        static inline bool busyRX();
        static inline void dispatchIRQ();
    };

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::sendIRQ(uint8_t *data, uint16_t size)
    {
        //TODO here
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::recvIRQ(uint8_t *data, uint16_t size)
    {
        //TODO here
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::sendDMA(uint8_t *data, uint16_t size)
    {
        while (busyTX())
            asm("nop");

        txState = State::BUSY;

        DMAtxT::clrFlagTC();

        regs()->CR1 |= USART_CR1_TCIE;
        regs()->CR3 |= USART_CR3_DMAT;

        DMAtxT::setTransferCallback([]() { txState = State::READY; });
        DMAtxT::transfer(DMA::Config::MEM_2_PER | DMA::Config::MINC, data, &regs()->DR, size);
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::recvDMA(uint8_t *data, uint16_t size)
    {
        while (busyRX())
            asm("nop");

        rxState = State::BUSY;

        DMArxT::clrFlagTC();

        regs()->CR1 |= USART_CR1_IDLEIE | USART_CR1_PEIE;
        regs()->CR3 |= USART_CR3_DMAR | USART_CR3_EIE;

        DMArxT::setTransferCallback([]() { rxState = State::READY; });
        DMArxT::transfer(DMA::Config::PER_2_MEM | DMA::Config::MINC, data, &regs()->DR, size);
    }

    UART_TPL_ARGUMENTS
    bool Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::busyTX()
    {
        return State::BUSY == txState;
    }

    UART_TPL_ARGUMENTS
    bool Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::busyRX()
    {
        return State::BUSY == rxState;
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::dispatchIRQ()
    {
        // all cases & notify upper layer
    }
}
