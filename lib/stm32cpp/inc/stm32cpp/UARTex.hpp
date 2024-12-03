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
#if defined(USART_ISR_PE)

#define USART_TYPE_1
#define REG_STATUS ISR
#define REG_TX_DATA TDR
#define REG_RX_DATA RDR

#endif

#if defined(USART_SR_PE)

#define USART_TYPE_2
#define REG_STATUS SR
#define REG_TX_DATA DR
#define REG_RX_DATA DR

#endif

    struct Config
    {
    };

    // to defs
    // SR = F1
    // ISR = G0
    enum IRQFlags
    {
        NONE = 0,
#ifdef USART_SR_PE
        PARITY_ERROR = USART_SR_PE,
        TX_EMPTY = USART_SR_TXE,
        TX_COMPLETE = USART_SR_TC,
        RX_NOT_EMPTY = USART_SR_RXNE,
        IDLE = USART_SR_IDLE,
        LINE_BREAK = USART_SR_LBD,
        CTS = USART_SR_CTS,
        ERRORS = USART_SR_PE | USART_SR_FE | USART_SR_NE | USART_SR_ORE,
        ALL = ERRORS | TX_EMPTY | TX_COMPLETE | RX_NOT_EMPTY | IDLE | LINE_BREAK | CTS,
#endif
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
        static inline IRQFlags getIRQFlags()
        {
#ifdef USART_SR_PE
            return static_cast<IRQFlags>(regs()->SR & IRQFlags::ALL);
#else
            return static_cast<IRQFlags>(regs()->ISR & IRQFlags::ALL);
#endif
        }

        static inline void clrIRQFlags(IRQFlags flags)
        {
#ifdef USART_SR_PE
            regs()->SR &= ~flags;
#else
            regs()->ICR = flags;
#endif
        }

        // enable UART, enable IRQ vector
        static void enable();
        // disable UART, disable IRQ vector
        static void disable();
        // configure bus
        static void configure(Config config);
        static inline void sendDMA(void *data, uint16_t size);
        static inline void recvDMA(void *data, uint16_t size);
        static inline bool busyTX();
        static inline bool busyRX();
        static inline void dispatchIRQ();
    };

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::sendDMA(void *data, uint16_t size)
    {
        while (busyTX())
            asm("nop");

        txState = State::BUSY;

        DMAtxT::clrFlagTC();

        regs()->CR1 |= USART_CR1_TCIE;
        regs()->CR3 |= USART_CR3_DMAT;

        DMAtxT::setTransferCallback([]()
                                    { txState = State::READY; });
        DMAtxT::transfer(DMA::Config::MEM_2_PER | DMA::Config::MINC, data, &regs()->DR, size);
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::recvDMA(void *data, uint16_t size)
    {
        while (busyRX())
            asm("nop");

        rxState = State::BUSY;

        DMArxT::clrFlagTC();

        regs()->CR1 |= USART_CR1_IDLEIE | USART_CR1_PEIE;
        regs()->CR3 |= USART_CR3_DMAR | USART_CR3_EIE;

        DMArxT::setTransferCallback([]()
                                    { rxState = State::READY; });
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
