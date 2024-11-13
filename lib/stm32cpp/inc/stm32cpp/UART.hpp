#ifndef __STM32_UART_HPP__
#define __STM32_UART_HPP__

#include <functional>
#include <stdint.h>
#include <string.h>
#include <stm32f1xx_hal_uart.h>
#include <stm32cpp/_common.hpp>
#include <stm32cpp/Atomic.hpp>
#include <stm32cpp/Clock.hpp>

namespace STM32
{
    namespace UART
    {
        enum class State
        {
            RESET,
            INITIALIZED,
            READY,
            BUSY,
            BUSY_TX,
            BUSY_RX,
            BUSY_TX_RX,
            ERROR,
            ABORT,
        };

        enum class Mode
        {
            NONE = 0x00000000u,
            RX = USART_CR1_RE,
            TX = USART_CR1_TE,
            RX_TX = USART_CR1_RE | USART_CR1_TE,
        };

        enum class StopBits
        {
            _1BIT = 0x00000000u,
            _2BIT = USART_CR2_STOP_1,
        };

        enum class DataBits
        {
            _8BIT = 0x00000000u,
            _9BIT = USART_CR1_M,
        };

        enum class Parity
        {
            NONE = 0x00000000u,
            EVEN = USART_CR1_PCE,
            ODD = USART_CR1_PCE | USART_CR1_PS,
        };

        enum class HWControl
        {
            NONE = 0x00000000u,
            RTS = USART_CR3_RTSE,
            CTS = USART_CR3_CTSE,
            RTS_CTS = USART_CR3_RTSE | USART_CR3_CTSE,
        };

        enum class Oversampling
        {
            _16BIT = 0x00000000u,
#if defined (USART_CR1_OVER8)
            _8BIT = USART_CR1_OVER8,
#endif
        };

        // BAUD = 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
        template <
            Mode tMode,
            uint32_t tBaudRate,
            DataBits tDataBits,
            StopBits tStopBits,
            Parity tParity,
            HWControl tHWControl = HWControl::NONE,
            Oversampling tOversampling = Oversampling::_16BIT
        >
        struct Config
        {
            static constexpr auto mode = tMode;
            static constexpr auto baudRate = tBaudRate;
            static constexpr auto dataBits = tDataBits;
            static constexpr auto stopBits = tStopBits;
            static constexpr auto parity = tParity;
            static constexpr auto hwControl = tHWControl;
            static constexpr auto oversampling = tOversampling;
        };

        typedef void(*ErrorHandlerT)(void);
        typedef void(*TXDoneHandlerT)(void);
        typedef void(*RXDoneHandlerT)(void);
        typedef void(*RXIdleHandlerT)(size_t);

        //TODO split to separate tx/rx
        struct Data {
            uint8_t* txBuf;
            size_t txLen;
            uint8_t* rxBuf;
            size_t rxLen;
            size_t rxCnt;
            TXDoneHandlerT txDoneHandler;
            RXDoneHandlerT rxDoneHandler;
            RXIdleHandlerT rxIdleHandler;
        };

        template <uint32_t TRegsAddress, IRQn_Type TEventIRQn, class TClock, class TReset>
        class Driver
        {
        private:
            static inline Data _data;
            static inline ErrorHandlerT _errorHandler;

            static constexpr USART_TypeDef* _regs()
            {
                return reinterpret_cast<USART_TypeDef*>(TRegsAddress);
            }

        public:
            template <class TConfig>
            static inline void init()
            {
                static constexpr const uint32_t mode = static_cast<uint32_t>(TConfig::mode);
                static constexpr const uint32_t baudRate = TConfig::baudRate;
                static constexpr const uint32_t dataBits = static_cast<uint32_t>(TConfig::dataBits);
                static constexpr const uint32_t stopBits = static_cast<uint32_t>(TConfig::stopBits);
                static constexpr const uint32_t parity = static_cast<uint32_t>(TConfig::parity);
                static constexpr const uint32_t hwControl = static_cast<uint32_t>(TConfig::hwControl);

                TClock::enable();
                TReset::reset();

                HAL_NVIC_SetPriority(TEventIRQn, 0, 0);
                HAL_NVIC_EnableIRQ(TEventIRQn);

                _regs()->CR1 &= ~USART_CR1_UE;

                MODIFY_REG(_regs()->CR2, USART_CR2_STOP, stopBits);

#if defined(USART_CR1_OVER8)
                static constexpr uint32_t oversampling = static_cast<uint32_t>(C::oversampling);
                MODIFY_REG(
                    _regs()->CR1,
                    (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
                    (dataBits | parity | mode | oversampling)
                );
#else
                MODIFY_REG(
                    _regs()->CR1,
                    (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE),
                    (dataBits | parity | mode)
                );
#endif /* USART_CR1_OVER8 */

                MODIFY_REG(_regs()->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), hwControl);

                uint32_t pclk;
                if constexpr (TRegsAddress == USART1_BASE) {
                    pclk = HAL_RCC_GetPCLK2Freq();
                }
                else {
                    pclk = HAL_RCC_GetPCLK1Freq();
                }

#if defined(USART_CR1_OVER8)
                if constexpr (C::oversampling == Oversampling::_8BIT) {
                    _regs()->BRR = UART_BRR_SAMPLING8(pclk, baudRate);
                }
                else {
                    _regs()->BRR = UART_BRR_SAMPLING16(pclk, baudRate);
                }
#else
                _regs()->BRR = UART_BRR_SAMPLING16(pclk, baudRate);
#endif /* USART_CR1_OVER8 */

                CLEAR_BIT(_regs()->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
                CLEAR_BIT(_regs()->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

                _regs()->CR1 |= USART_CR1_UE;
            }

            static void deinit()
            {
                _regs()->CR1 &= ~USART_CR1_UE;

                TClock::disable();
                NVIC_DisableIRQ(TEventIRQn);
            }

            static inline void setErrorHandler(void (*handler)(void))
            {
                _errorHandler = handler;
            }

            static inline void listen(uint8_t* buf, size_t len, RXIdleHandlerT cb)
            {
                Atomic::CompareExchange(&_data.rxBuf, (uint8_t*)nullptr, buf);

                _data.rxLen = len;
                _data.rxCnt = 0;

                _data.rxIdleHandler = cb;

                _regs()->CR1 |= USART_CR1_PEIE | USART_CR1_RXNEIE | USART_CR1_IDLEIE;
                _regs()->CR3 |= USART_CR3_EIE;
            }

            static inline void send(uint8_t* data, size_t size, TXDoneHandlerT cb)
            {
                // Just set data pointer to buf provide undefined behaviour, need use cortex commands
                Atomic::CompareExchange(&_data.txBuf, (uint8_t*)nullptr, data);

                _data.txLen = size;

                _data.txDoneHandler = cb;

                _regs()->CR1 |= USART_CR1_TXEIE;
            }

            static inline void recv(uint8_t* data, size_t size, RXDoneHandlerT cb)
            {
                _data.rxBuf = data;
                _data.rxLen = size;
                _data.rxCnt = 0;

                _data.rxDoneHandler = cb;

                _regs()->CR1 |= USART_CR1_PEIE;
                _regs()->CR3 |= USART_CR3_EIE;
                _regs()->CR1 |= USART_CR1_RXNEIE;
            }

            static inline void dispatchIRQ()
            {
                uint32_t SR = _regs()->SR;
                uint32_t CR1 = _regs()->CR1;
                uint32_t CR3 = _regs()->CR3;

                uint32_t errors = SR & (USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE);
                if (errors == 0u) {
                    if ((SR & USART_SR_RXNE) != 0u && (CR1 & USART_CR1_RXNEIE) != 0u) {
                        _onRXNE();
                        return;
                    }
                }
                if (errors != 0u && ((CR3 & USART_CR3_EIE) != 0u || (CR1 & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != 0u)) {
                    if ((SR & USART_SR_RXNE) != 0u && (CR1 & USART_CR1_RXNEIE) != 0u) {
                        _onRXNE();
                    }
                    if ((SR & USART_SR_ORE) != 0u && ((CR1 & USART_CR1_RXNEIE) != 0u || (CR3 & USART_CR3_EIE) != 0u)) {
                        _endRX();
                    }
                    if (_errorHandler) {
                        _errorHandler();
                    }
                    return;
                }
                if ((SR & USART_SR_IDLE) != 0u && (CR1 & USART_CR1_IDLEIE) != 0u) {
                    _clearSeq();
                    _endRX();
                    _regs()->CR1 &= ~USART_CR1_IDLEIE;
                    _data.rxBuf = nullptr;
                    if (_data.rxIdleHandler) {
                        _data.rxIdleHandler(_data.rxCnt);
                    }
                    return;
                }
                if (((SR & USART_SR_TXE) != 0u) && ((CR1 & USART_CR1_TXEIE) != 0u)) {
                    _onTXE();
                    return;
                }
                if (((SR & USART_SR_TC) != 0u) && ((CR1 & USART_CR1_TCIE) != 0u)) {
                    _endTX();
                    return;
                }
            }

        private:
            static inline void _clearSeq()
            {
                __IO uint32_t tmpreg = 0x00U;
                tmpreg = _regs()->SR;
                tmpreg = _regs()->DR;
                UNUSED(tmpreg);
            }
            static inline void _onRXNE()
            {
                *_data.rxBuf = (uint8_t)_regs()->DR;

                _data.rxBuf++;
                _data.rxCnt++;
                _data.rxLen--;

                if (_data.rxLen == 0) {
                    _data.rxBuf = nullptr;
                    _endRX();
                    if (_data.rxDoneHandler) {
                        _data.rxDoneHandler();
                    }
                }
            }
            static inline void _onTXE()
            {
                _regs()->DR = (uint8_t)(*_data.txBuf);

                _data.txBuf++;
                _data.txLen--;

                if (_data.txLen == 0) {
                    _data.txBuf = nullptr;
                    _regs()->CR1 &= ~USART_CR1_TXEIE;
                    _regs()->CR1 |= USART_CR1_TCIE;
                }
            }
            static inline void _endRX()
            {
                _regs()->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_PEIE);
                _regs()->CR3 &= ~(USART_CR3_EIE);
            }
            static inline void _endTX()
            {
                _regs()->CR1 &= ~USART_CR1_TCIE;
                if (_data.txDoneHandler) {
                    _data.txDoneHandler();
                }
            }
        };
    }

    /**
     * FAMILY |  F1  |  F2  |  F3  |  F4
     * UART1  | APB2 | APB2 | APB2 | APB2
     * UART2  | APB1 | APB1 | APB1 | APB1
     * UART3  | APB1 | APB1 | APB1 | APB1
     * UART4  | APB1 | APB1 | APB1 | APB1
     * UART5  | APB1 | APB1 | APB1 | APB1
     * UART6  | ---- | APB2 | ---- | APB2
     * UART7  | ---- | ---- | ---- | APB1
     * UART8  | ---- | ---- | ---- | APB1
     */
#if defined(USART1_BASE)
    using UART1_Driver = UART::Driver<
        USART1_BASE,
        USART1_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_USART1EN>,
        Clock::ResetControl<&RCC_TypeDef::APB2RSTR, RCC_APB2RSTR_USART1RST>
    >;
#endif
#if defined(USART2_BASE)
    using UART2_Driver = UART::Driver<
        USART2_BASE,
        USART2_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART2EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART2RST>
    >;
#endif
#if defined(USART3_BASE)
    using UART3_Driver = UART::Driver<
        USART3_BASE,
        USART3_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART3EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART3RST>
    >;
#endif
#if defined(UART4_BASE)
    using UART4_Driver = UART::Driver<
        UART4_BASE,
        UART4_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_UART4EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_UART4RST>
    >;
#elif defined(USART4_BASE)
    using UART4_Driver = UART::Driver<
        USART4_BASE,
        USART4_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART4EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART4RST>
    >;
#endif
#if defined(UART5_BASE)
    using UART5_Driver = UART::Driver<
        UART5_BASE,
        UART5_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_UART5EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_UART5RST>
    >;
#elif defined(USART5_BASE)
    using UART5_Driver = UART::Driver<
        USART5_BASE,
        USART5_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART5EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART5RST>
    >;
#endif
#if defined(USART6_BASE)
    using UART6_Driver = UART::Driver<
        USART6_BASE,
        USART6_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_USART6EN>,
        Clock::ResetControl<&RCC_TypeDef::APB2RSTR, RCC_APB2RSTR_USART6RST>
    >;
#endif
#if defined(UART7_BASE)
    using UART7_Driver = UART::Driver<
        UART7_BASE,
        UART7_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART7EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART7RST>
    >;
#elif defined(USART7_BASE)
    using UART7_Driver = UART::Driver<
        USART7_BASE,
        USART7_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART7EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART7RST>
    >;
#endif
#if defined(UART8_BASE)
    using UART8_Driver = UART::Driver<
        UART8_BASE,
        UART8_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_UART8EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_UART8RST>
    >;
#elif defined(USART8_BASE)
    using UART8_Driver = UART::Driver<
        USART8_BASE,
        USART8_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_USART8EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_USART8RST>
    >;
#endif
}

#endif // __STM32_UART_HPP__
