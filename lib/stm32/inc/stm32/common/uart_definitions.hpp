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
        ALL = ERRORS | TX_EMPTY | TX_COMPLETE | RX_NOT_EMPTY | IDLE | LINE_BREAK | CTS,
#endif
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
