#pragma once

namespace STM32::UART
{
    enum Flag : uint32_t
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
#endif
#ifdef USART_ISR_PE
        PARITY_ERROR = USART_ISR_PE,
        TX_EMPTY = USART_ISR_TXE,
        TX_COMPLETE = USART_ISR_TC,
        RX_NOT_EMPTY = USART_ISR_RXNE,
        IDLE = USART_ISR_IDLE,
#ifdef USART_ISR_LBD
        LINE_BREAK = USART_ISR_LBD,
#else
        LINE_BREAK = 0,
#endif
        CTS = USART_ISR_CTS,
        ERRORS = USART_ISR_FE | USART_ISR_NE | USART_ISR_ORE,
#ifdef USART_CR1_FIFOEN
        RX_FIFO_FULL = USART_ISR_RXFF,
        TX_FIFO_EMPTY = USART_ISR_TXFE,
        RX_FIFO_THRESHOLD = USART_ISR_RXFT,
        TX_FIFO_THRESHOLD = USART_ISR_TXFT,
#endif
#ifdef USART_CR2_RTOEN
        RX_TIMEOUT = USART_ISR_RTOF,
#endif
#endif
        ALL = ERRORS | TX_EMPTY | TX_COMPLETE | RX_NOT_EMPTY | IDLE | LINE_BREAK | CTS
    };

    template <
        uint32_t tRegsAddr,
        IRQn_Type tIRQn,
        typename tClock,
        typename tDMATx,
        typename tDMARx
    >
    class Driver
    {
    private:
        static constexpr USART_TypeDef* regs();

    public:
        static inline void configure();
        static inline bool send(void *data, uint16_t size);
        static inline bool recv(void *data, uint16_t size);
        static inline bool busyTx();
        static inline bool busyRx();
        template <Flag tFlag>
        static inline bool hasFlag();
        template <Flag tFlag>
        static inline void clrFlag();
    };
}
