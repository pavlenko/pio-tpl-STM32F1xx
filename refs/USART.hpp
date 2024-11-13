#ifndef __STM32_USART_HPP__
#define __STM32_USART_HPP__

#include <functional>
#include <stm32f1xx.h>

#include "_common.hpp"

//TODO see https://github.com/BojanSof/stm32f4x1-cpp-libs as reference
namespace STM32
{
    enum USART_INDEX {
        USART1_INDEX,
#if defined(USART2_BASE)
        USART2_INDEX,
#endif
#if defined(USART3_BASE)
        USART3_INDEX,
#endif
#if defined(USART4_BASE)
        USART4_INDEX,
#endif
#if defined(USART5_BASE)
        USART5_INDEX,
#endif
#if defined(USART6_BASE)
        USART6_INDEX,
#endif
#if defined(USART7_BASE)
        USART7_INDEX,
#endif
#if defined(USART8_BASE)
        USART8_INDEX,
#endif
        USART_NUM,
    };

    enum class USART_Status
    {
        RESET      = 0x00U,
        READY      = 0x20U,
        BUSY       = 0x24U,
        BUSY_TX    = 0x21U,
        BUSY_RX    = 0x22U,
        BUSY_TX_RX = 0x23U,
        TIMEOUT    = 0xA0U,
        ERROR      = 0xE0U,
    };

    enum class USART_Error: uint8_t {
        NONE    = 0x00U,
        PARITY  = 0x01U,
        NOISE   = 0x02U,
        FRAME   = 0x04U,
        OVERRUN = 0x08U,
        DMA     = 0x10U,
    };

    inline bool operator!=(const uint8_t lft, const USART_Error rgt)
    {
        return lft != static_cast<uint8_t>(rgt);
    }

    inline uint8_t operator|=(const uint8_t lft, const USART_Error rgt)
    {
        return lft | static_cast<uint8_t>(rgt);
    }

    inline uint8_t operator&(const uint8_t lft, const USART_Error rgt)
    {
        return lft & static_cast<uint8_t>(rgt);
    }

    enum class USART_Mode: uint16_t
    {
        RX    = USART_CR1_RE,
        TX    = USART_CR1_TE,
        TX_RX = (USART_CR1_TE | USART_CR1_RE),
    };

    enum class USART_DataBits: uint16_t
    {
        DATA_8BIT = 0x00000000U,
        DATA_9BIT = USART_CR1_M,
    };

    enum class USART_StopBits: uint16_t
    {
        STOP_1BIT = 0x00000000U,
        STOP_2BIT = USART_CR2_STOP_1,
    };

    enum class USART_Parity: uint16_t
    {
        NONE = 0x00000000U,
        EVEN = USART_CR1_PCE,
        ODD  = (USART_CR1_PCE | USART_CR1_PS),
    };

    class USART_Config
    {
        template<uint8_t index> friend class USART;
    private:
        uint32_t _baud;
        USART_Mode _mode;
        USART_DataBits _dataBits;
        USART_StopBits _stopBits;
        USART_Parity _parity;
    public:
        constexpr USART_Config()
            : _baud(9600),
              _mode(USART_Mode::TX_RX),
              _dataBits(USART_DataBits::DATA_8BIT),
              _stopBits(USART_StopBits::STOP_1BIT),
              _parity(USART_Parity::NONE)
        {}
        constexpr USART_Config(uint32_t baud, USART_Mode m, USART_DataBits db, USART_StopBits sb, USART_Parity parity)
            : _baud(baud), _mode(m), _dataBits(db), _stopBits(sb), _parity(parity)
        {}
    };

    template <uint8_t index>
    class USART
    {
    private:
        volatile USART_Status _status;
        volatile uint8_t _errors;
        USART(): _errors(0U)
        {
            static_assert(index >= 1 && index <= USART_NUM, "Invalid UART index");

            IRQn_Type irqn;
            if constexpr(index == 1) {
                __HAL_RCC_USART1_CLK_ENABLE();
                irqn = USART1_IRQn;
            }
#if defined(USART2_BASE)
            else if constexpr(index == 2) {
                __HAL_RCC_USART2_CLK_ENABLE();
                irqn = USART1_IRQn;
            }
#endif
#if defined(USART3_BASE)
            else if constexpr(index == 3) {
                __HAL_RCC_USART3_CLK_ENABLE();
                irqn = USART3_IRQn;
            }
#endif
#if defined(USART4_BASE)
            else if constexpr(index == 4) {
                __HAL_RCC_USART4_CLK_ENABLE();
                irqn = USART4_IRQn;
            }
#endif
#if defined(USART5_BASE)
            else if constexpr(index == 5) {
                __HAL_RCC_USART5_CLK_ENABLE();
                irqn = USART5_IRQn;
            }
#endif
#if defined(USART6_BASE)
            else if constexpr(index == 6) {
                __HAL_RCC_USART6_CLK_ENABLE();
                irqn = USART6_IRQn;
            }
#endif
#if defined(USART7_BASE)
            else if constexpr(index == 7) {
                __HAL_RCC_USART7_CLK_ENABLE();
                irqn = USART7_IRQn;
            }
#endif
#if defined(USART8_BASE)
            else if constexpr(index == 8) {
                __HAL_RCC_USART8_CLK_ENABLE();
                irqn = USART8_IRQn;
            }
#endif

            // errorIRQHandler = [this](void){
            //     uint32_t SR    = _regs->SR;
            //     uint32_t CR1   = _regs->CR1;
            //     uint32_t CR3   = _regs->CR3;
            //     uint32_t flags = (SR & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
            //     if (flags != 0U) {
            //         if ((flags & USART_SR_PE) != 0U && (CR1 & USART_CR1_PEIE) != 0U) {
            //             _errors |= HAL_UART_ERROR_PE;
            //         }
            //         if ((flags & USART_SR_NE) != 0U && (CR3 & USART_CR3_EIE) != 0U) {
            //             _errors |= HAL_UART_ERROR_NE;
            //         }
            //         if ((flags & USART_SR_FE) != 0U && (CR3 & USART_CR3_EIE) != 0U) {
            //             _errors |= HAL_UART_ERROR_FE;
            //         }
            //         if ((flags & USART_SR_ORE) != 0U && ((CR1 & USART_CR1_RXNEIE) != 0U || (CR3 & USART_CR3_EIE) != 0U)) {
            //             _errors |= HAL_UART_ERROR_ORE;
            //         }
            //         if (_errors) {
            //             while (!(this->_regs->SR & USART_SR_TC));//<--wait for tx complete???
            //             this->_regs->CR1 &= ~USART_CR1_UE;//<--disable periph???
            //             if (this->_errorCallback) {
            //                 this->_errorCallback();
            //             }
            //             eventIRQHandler = nullptr;
            //             this->detachTxIRQ();
            //             this->detachRxIRQ();
            //         }
            //     }
            // };

            NVIC_ClearPendingIRQ(irqn);
            NVIC_EnableIRQ(irqn);

            _regs->CR3 |= USART_CR3_EIE;//<-- are this needs here, because it disables in method
        }

        USART(const USART&) = delete;
        USART(USART&&) = delete;
        void operator=(const USART&) = delete;
        void operator=(USART&&) = delete;

        static constexpr USART_TypeDef *getRegs()
        {
            if constexpr (index == 1)
            {
                return USART1;
            }
#if defined(USART2_BASE)
            else if constexpr (index == 2)
            {
                return USART2;
            }
#endif
#if defined(USART3_BASE)
            else if constexpr (index == 3)
            {
                return USART3;
            }
#endif
#if defined(USART4_BASE)
            else if constexpr (index == 4)
            {
                return USART4;
            }
#endif
#if defined(USART5_BASE)
            else if constexpr (index == 5)
            {
                return USART5;
            }
#endif
#if defined(USART6_BASE)
            else if constexpr (index == 6)
            {
                return USART6;
            }
#endif
#if defined(USART7_BASE)
            else if constexpr (index == 7)
            {
                return USART7;
            }
#endif
#if defined(USART8_BASE)
            else if constexpr (index == 8)
            {
                return USART8;
            }
#endif
            else
            {
                return nullptr;
            }
        }

        USART_TypeDef *const _regs = getRegs();

        std::function<void(void)> _eventCallback;
        std::function<void(void)> _errorCallback;

        USART_Config _config;

        uint8_t *_txDataBuf;
        __IO uint16_t _txDataCnt;
        uint8_t *_rxDataBuf;
        __IO uint16_t _rxDataCnt;

        inline void _txIRQ(void);

    public:
        inline static std::function<void(void)> eventIRQHandler;
        inline static std::function<void(void)> errorIRQHandler;

        static USART &getInstance()
        {
            static USART instance;
            return instance;
        }

        Status configure(const USART_Config config)
        {
            if (_status != USART_Status::RESET) {
                return Status::ERROR;
            }

            _status = USART_Status::BUSY;
            _config = config;

            disable();

            MODIFY_REG(_regs->CR2, USART_CR2_STOP, static_cast<uint32_t>(_config._stopBits));

            MODIFY_REG(_regs->CR1, USART_CR1_M, static_cast<uint32_t>(_config._dataBits));
            MODIFY_REG(_regs->CR1, (USART_CR1_PCE | USART_CR1_PS), static_cast<uint32_t>(_config._parity));
            MODIFY_REG(_regs->CR1, (USART_CR1_TE | USART_CR1_RE), static_cast<uint32_t>(_config._mode));

            uint32_t pclk;
            if constexpr (index == 1) {
                pclk = HAL_RCC_GetPCLK2Freq();
            }
            else
            {
                pclk = HAL_RCC_GetPCLK1Freq();
            }

            //TODO optimize for compile time
            _regs->BRR = UART_BRR_SAMPLING16(pclk, static_cast<uint32_t>(_config._baud));

            CLEAR_BIT(_regs->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
            CLEAR_BIT(_regs->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

            enable();

            _errors = 0U;
            _status = USART_Status::READY;

            return Status::OK;
        }

        //TODO need more usable api for allow handle per char reception
        Status send(uint8_t *data, uint16_t size); /*TODO IRQ/DMA*/
        Status recv(uint8_t *data, uint16_t size); /*TODO IRQ/DMA*/
        Status abort();                            /*TODO IRQ/DMA*/

        Status listen(std::function<void(uint16_t)> cb)
        {
            if (_status != USART_Status::READY) {
                return Status::BUSY;
            }

            //TODO for this:
            //set status to busy
            //set rx buffer depends on data length
            //maybe better:
            // uint16_t data; uint16_t size = 1; recv(&data, 1)

            //TODO extract common...??? better - create dispatchIRQ method & move all logic to it
            // also maybe extract common logic to helper functions like in HAL instead of use lambda
            _eventCallback = [this, cb](void){
                if ((_regs->SR & USART_SR_RXNE) != 0U && (_regs->CR1 & USART_CR1_RXNEIE) != 0U) {
                    //TODO common receive cb?
                    uint16_t dataRx = 0;
                    if (/*data=9b && parity=none*/true) {
                        dataRx = _regs->DR & 0x01FFU;
                    } else {
                        if (/*data=9b || (data=8b && parity=none)*/true) {
                            dataRx = _regs->DR & 0x00FFU;
                        } else {
                            dataRx = _regs->DR & 0x007FU;
                        }
                    }
                    cb(dataRx);
                    //TODO if buffer empty - disable rxne, pe, err interrupts; set state to ready; call callback;
                }
                if ((_regs->SR & USART_SR_TXE) != 0U && (_regs->CR1 & USART_CR1_TXEIE) != 0U) {
                    //TODO transmit cb
                    uint16_t dataTx = 0x9F;
                    if (/*data=9b && parity=none*/true) {
                        _regs->DR = dataTx & 0x01FFU;
                    } else {
                        _regs->DR = dataTx & 0x00FFU;
                    }
                    //TODO: if buffer empty - disable txe interrupt, enable tc interrupt
                    return;
                }
                if (((_regs->SR & USART_SR_TC) != 0U) && ((_regs->CR1 & USART_CR1_TCIE) != 0U)) {
                    _regs->CR1 &= ~USART_CR1_TCIE;
                    _status = USART_Status::READY;
                    //TODO tx completed callback
                    return;
                }
            };

            _status = USART_Status::BUSY_RX;

            _regs->CR1 |= USART_CR1_RXNEIE | USART_CR1_PEIE;
            _regs->CR3 |= USART_CR3_EIE;

            return Status::OK;
        }

        inline void reset(void);

        inline void enable(void)
        {
            _regs->CR1 |= USART_CR1_UE;
        }

        inline void disable(void)
        {
            _regs->CR1 &= ~USART_CR1_UE;
        }

        inline void attachTxIRQ(void)
        {
            _regs->CR1 |= (USART_CR1_TXEIE | USART_CR1_TCIE);
        }

        inline void detachTxIRQ(void)
        {
            _regs->CR1 &= ~(USART_CR1_TXEIE | USART_CR1_TCIE);
        }

        inline void attachRxIRQ(void)
        {
            _regs->CR1 |= (USART_CR1_RXNEIE | USART_CR1_PEIE);
            _regs->CR3 |= USART_CR3_EIE;//<--maybe remove from this
        }

        inline void detachRxIRQ(void)
        {
            _regs->CR1 &= ~(USART_CR1_RXNEIE | USART_CR1_PEIE);
            _regs->CR3 &= ~USART_CR3_EIE;//<--maybe remove from this
        }

        inline void attachTxDMA(void *channel) { (void) channel; }
        inline void attachRxDMA(void *channel) { (void) channel; }

        inline void dispatchIRQ(void);
    };
}

#endif // __STM32_USART_HPP__