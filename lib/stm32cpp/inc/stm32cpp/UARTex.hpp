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

    // to defs
    // SR = F1
    // ISR = G0
    enum IRQFlags : uint32_t
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
        static inline bool m_busyTX;
        static inline bool m_busyRX;
        static constexpr USART_TypeDef *regs() { return reinterpret_cast<USART_TypeDef *>(RegsT); }

    public:
        static inline void enable();
        static inline void disable();
        static inline void configure(uint32_t baud, const Config2 config = Config2());
        static inline void sendDMA(void *data, uint16_t size);
        static inline void recvDMA(void *data, uint16_t size);
        static inline bool busyTX();
        static inline bool busyRX();
        static inline IRQFlags getIRQFlags();
        static inline void clrIRQFlags(IRQFlags flags);
        static inline void dispatchIRQ();

    private:
        static inline void endTX();
        static inline void endRX();
    };

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::configure(uint32_t baud, const Config2 config)
    {
        static constexpr const uint32_t mode = static_cast<uint32_t>(config.mode);
        static constexpr const uint32_t dataBits = static_cast<uint32_t>(config.dataBits);
        static constexpr const uint32_t stopBits = static_cast<uint32_t>(config.stopBits);
        static constexpr const uint32_t parity = static_cast<uint32_t>(config.parity);
        static constexpr const uint32_t hwControl = static_cast<uint32_t>(config.hwControl);
        static constexpr const uint32_t oversampling = static_cast<uint32_t>(config.oversampling);

        uint32_t pclk;
        if constexpr (RegsT == USART1_BASE)
        {
            pclk = HAL_RCC_GetPCLK2Freq();
        }
        else
        {
            pclk = HAL_RCC_GetPCLK1Freq();
        }

#if defined(USART_CR1_OVER8)
        if constexpr (oversampling == Oversampling::_8BIT)
        {
            regs()->BRR = UART_BRR_SAMPLING8(pclk, baud);
        }
        else
        {
            regs()->BRR = UART_BRR_SAMPLING16(pclk, baud);
        }
#else
        regs()->BRR = UART_BRR_SAMPLING16(pclk, baud);
#endif /* USART_CR1_OVER8 */

        regs()->CR1 = mode | dataBits | parity | oversampling;
        regs()->CR2 = stopBits;
        regs()->CR3 = hwControl;
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::enable()
    {
        ClockT::enable();
        NVIC_EnableIRQ(IRQnT);
        regs()->CR1 |= USART_CR1_UE;
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::disable()
    {
        ClockT::disable();
        NVIC_DisableIRQ(IRQnT);
        regs()->CR1 &= ~USART_CR1_UE;
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::sendDMA(void *data, uint16_t size)
    {
        while (busyTX())
            asm("nop");

        m_busyTX = true;

        DMAtxT::clrFlagTC();

        regs()->CR3 |= USART_CR3_DMAT;

        DMAtxT::setTransferCallback(endTX);//TODO fix callback signature
        DMAtxT::transfer(DMA::Config::MEM_2_PER | DMA::Config::MINC, data, &regs()->REG_TX_DATA, size);
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::recvDMA(void *data, uint16_t size)
    {
        while (busyRX())
            asm("nop");

        m_busyRX = true;

        DMArxT::clrFlagTC();

        regs()->CR1 |= USART_CR1_IDLEIE;
        regs()->CR3 |= USART_CR3_DMAR;

        DMArxT::setTransferCallback(endRX);
        DMArxT::transfer(DMA::Config::PER_2_MEM | DMA::Config::MINC, data, &regs()->REG_RX_DATA, size);
    }

    UART_TPL_ARGUMENTS
    bool Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::busyTX()
    {
        return m_busyTX;
    }

    UART_TPL_ARGUMENTS
    bool Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::busyRX()
    {
        return m_busyRX;
    }

    UART_TPL_ARGUMENTS
    IRQFlags Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::getIRQFlags()
    {
#ifdef USART_SR_PE
        return static_cast<IRQFlags>(regs()->SR & IRQFlags::ALL);
#else
        return static_cast<IRQFlags>(regs()->ISR & IRQFlags::ALL);
#endif
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::clrIRQFlags(IRQFlags flags)
    {
#ifdef USART_SR_PE
        regs()->SR &= ~flags;
#else
        regs()->ICR = flags;
#endif
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::dispatchIRQ()
    {
        if (getIRQFlags() & IRQFlags::IDLE)
        {
            clrIRQFlags(IRQFlags::IDLE);
            endRX();
        }
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::endTX()
    {
        regs()->CR3 &= ~USART_CR3_DMAT;
        DMAtxT::abort();
        m_busyTX = false;
    }

    UART_TPL_ARGUMENTS
    void Driver<RegsT, IRQnT, ClockT, DMAtxT, DMArxT>::endRX()
    {
        regs()->CR1 &= ~USART_CR1_IDLEIE;
        regs()->CR3 &= ~USART_CR3_DMAR;
        DMArxT::abort();
        m_busyRX = false;
    }
}
