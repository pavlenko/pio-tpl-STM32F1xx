#ifndef __STM32_I2C_HPP__
#define __STM32_I2C_HPP__

#include <functional>
#include <stdint.h>

#include "_common.hpp"
#include <stm32cpp/I2C_definitions.hpp>

namespace STM32
{
    namespace I2C
    {
        enum class N
        {
#if defined(I2C1_BASE)
            _1,
#endif
#if defined(I2C2_BASE)
            _2,
#endif
#if defined(I2C3_BASE)
            _3,
#endif
#if defined(I2C4_BASE)
            _4,
#endif
#if defined(I2C5_BASE)
            _5,
#endif
#if defined(I2C6_BASE)
            _6,
#endif
            TOTAL,
        };

        static_assert(static_cast<uint8_t>(N::TOTAL) != 0U, "Device doesn't support I2C");

        enum class State
        {
            RESET = 0x00U,
            READY = 0x20U,
            BUSY = 0x24U,
            BUSY_TX = 0x21U,
            BUSY_RX = 0x22U,
            LISTEN = 0x28U,
            BUSY_TX_LISTEN = 0x29U,
            BUSY_RX_LISTEN = 0x2AU,
            ABORT = 0x60U,
            TIMEOUT = 0xA0U,
            ERROR = 0xE0U,
        };

        enum class Error
        {
            NONE = 0x0000U,
            BERR = 0x0001U,
            ARLO = 0x0002U,
            AF = 0x0004U,
            OVR = 0x0008U,
        };

        template <uint32_t regs_base, IRQn_Type eventIRQn, IRQn_Type errorIRQn>
        class Driver
        {
        private:
            static constexpr I2C_TypeDef *__regs() { return reinterpret_cast<I2C_TypeDef *>(regs_base); };

            Driver() {}

            Driver(const Driver &) = delete;         // Prevent copy constructor
            Driver(Driver &&) = delete;              // Prevent move constructor
            void operator=(const Driver &) = delete; // Prevent assignment
            void operator=(Driver &&) = delete;      // Prevent reference

            I2C_TypeDef *const _regs = __regs();

            State _state;
            uint8_t _errors;
            uint8_t _devAddress;

            uint8_t *_dataBuf;
            uint16_t _dataLen; // why need both len & cnt, if first not used anywhere???
            uint16_t _dataCnt;

            uint16_t _addrVal;
            uint8_t _addrLen;

        public:
            /**
             * Construct a new Driver object singleton
             */
            static Driver &instance()
            {
                static Driver _instance;
                return _instance;
            }

            /**
             * Initialize peripherial
             */
            void init(Speed speed)
            {
#if defined(I2C1_BASE)
                if constexpr (regs_base == I2C1_BASE)
                {
                    __HAL_RCC_I2C1_CLK_ENABLE();
                    __HAL_RCC_I2C1_FORCE_RESET();
                    __HAL_RCC_I2C1_RELEASE_RESET();
                }
#endif
#if defined(I2C2_BASE)
                if constexpr (regs_base == I2C2_BASE)
                {
                    __HAL_RCC_I2C2_CLK_ENABLE();
                    __HAL_RCC_I2C2_FORCE_RESET();
                    __HAL_RCC_I2C2_RELEASE_RESET();
                }
#endif
#if defined(I2C3_BASE)
                if constexpr (regs_base == I2C3_BASE)
                {
                    __HAL_RCC_I2C3_CLK_ENABLE();
                    __HAL_RCC_I2C3_FORCE_RESET();
                    __HAL_RCC_I2C3_RELEASE_RESET();
                }
#endif
#if defined(I2C4_BASE)
                if constexpr (regs_base == I2C4_BASE)
                {
                    __HAL_RCC_I2C4_CLK_ENABLE();
                    __HAL_RCC_I2C4_FORCE_RESET();
                    __HAL_RCC_I2C4_RELEASE_RESET();
                }
#endif
#if defined(I2C5_BASE)
                if constexpr (regs_base == I2C5_BASE)
                {
                    __HAL_RCC_I2C5_CLK_ENABLE();
                    __HAL_RCC_I2C5_FORCE_RESET();
                    __HAL_RCC_I2C5_RELEASE_RESET();
                }
#endif
#if defined(I2C6_BASE)
                if constexpr (regs_base == I2C6_BASE)
                {
                    __HAL_RCC_I2C6_CLK_ENABLE();
                    __HAL_RCC_I2C6_FORCE_RESET();
                    __HAL_RCC_I2C6_RELEASE_RESET();
                }
#endif

                NVIC_ClearPendingIRQ(eventIRQn);
                NVIC_EnableIRQ(eventIRQn);

                NVIC_ClearPendingIRQ(errorIRQn);
                NVIC_EnableIRQ(errorIRQn);

                _state = State::BUSY;

                _regs->CR1 &= ~I2C_CR1_PE;

                _regs->CR1 |= I2C_CR1_SWRST;
                _regs->CR1 &= ~I2C_CR1_SWRST;

                uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
                uint32_t freqrange = I2C_FREQRANGE(pclk1);

                MODIFY_REG(_regs->CR1, I2C_CR1_SMBUS | I2C_CR1_SMBTYPE | I2C_CR1_ENARP, 0U);
                MODIFY_REG(_regs->CR2, I2C_CR2_FREQ, freqrange);
                MODIFY_REG(_regs->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, static_cast<uint32_t>(speed)));

                if (speed == Speed::STANDARD)
                {
                    MODIFY_REG(_regs->CCR, I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR, I2C_SPEED_STANDARD(pclk1, static_cast<uint32_t>(speed)));
                }
                else
                {
                    MODIFY_REG(_regs->CCR, I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR, I2C_CCR_CALCULATION(pclk1, static_cast<uint32_t>(speed), 25U) | I2C_DUTYCYCLE_16_9);
                }

                _regs->CR1 |= I2C_CR1_PE;

                _state = State::READY;
            }

            /**
             * De-initialize peripherial
             */
            void deinit()
            {
                _state = State::BUSY;

                _regs->CR1 &= ~I2C_CR1_PE;

#if defined(I2C1_BASE)
                if constexpr (regs_base == I2C1_BASE)
                    __HAL_RCC_I2C1_CLK_DISABLE();
#endif
#if defined(I2C2_BASE)
                if constexpr (regs_base == I2C2_BASE)
                    __HAL_RCC_I2C2_CLK_DISABLE();
#endif
#if defined(I2C3_BASE)
                if constexpr (regs_base == I2C3_BASE)
                    __HAL_RCC_I2C3_CLK_DISABLE();
#endif
#if defined(I2C4_BASE)
                if constexpr (regs_base == I2C4_BASE)
                    __HAL_RCC_I2C4_CLK_DISABLE();
#endif
#if defined(I2C5_BASE)
                if constexpr (regs_base == I2C5_BASE)
                    __HAL_RCC_I2C5_CLK_DISABLE();
#endif
#if defined(I2C6_BASE)
                if constexpr (regs_base == I2C6_BASE)
                    __HAL_RCC_I2C6_CLK_DISABLE();
#endif

                NVIC_DisableIRQ(eventIRQn);
                NVIC_DisableIRQ(errorIRQn);

                _state = State::RESET;
            }

            /**
             * Lister to address, automatically go to slave mode
             *
             * @param address
             * @param gc
             * @param ns
             * @param cb Event/error callback
             */
            void listen(uint8_t address, bool gc, bool ns, void (*cb)(Direction))
            {
                if (_state == State::READY)
                {
                    _state = State::LISTEN;

                    MODIFY_REG(_regs->CR1, I2C_CR1_ENGC | I2C_CR1_NOSTRETCH, (gc << I2C_CR1_ENGC_Pos) | (ns << I2C_CR1_NOSTRETCH_Pos));

                    _regs->OAR1 = (address & 0x7F) << 1U;
                    _regs->OAR2 = 0U;

                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 |= I2C_CR1_ACK;
                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN;
                }
            }

            /**
             * Select slave device, automatically go to master mode
             * - check if state = READY
             * - configure slave address for next operations
             *
             * @param address
             */
            void select(uint8_t address)
            {
                if (_state == State::READY)
                {
                    _devAddress = address & ~0x01U;
                    // TODO just toggle state for properly handle send/recv in master/slave
                    // TODO maybe set master event handler here
                }
            }

            /**
             * Send data to bus (async)
             * - check if state = READY or LISTEN
             * - prepare internal tx buffer
             * - enable tx interrupts + handler
             * - set state = BUSY_TX(_LISTEN(?))
             *
             * @param data
             * @param size
             * @param cb Event/error callback
             */
            void send(uint8_t *data, uint16_t size, void (*cb)(void))
            {
                // master: check for select state, set data buffer to send, send start, enable irq, set state to busy tx
                if (_state == State::READY)
                {
                    while (_regs->SR2 & I2C_SR2_BUSY)
                        ;

                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 &= ~I2C_CR1_POS;

                    _errors = static_cast<uint8_t>(Error::NONE);
                    _state = State::BUSY_TX;

                    _dataBuf = data;
                    _dataLen = size;
                    _dataCnt = size;

                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
                    _regs->CR1 |= I2C_CR1_START;
                }

                // slave: check for listen state, set data buffer to send, enable irq, set state to busy tx listen
                if (_state == State::LISTEN)
                {
                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 &= ~I2C_CR1_POS;

                    _errors = static_cast<uint8_t>(Error::NONE);
                    _state = State::BUSY_TX_LISTEN;

                    _dataBuf = data;
                    _dataLen = size;
                    _dataCnt = size;

                    // TODO clear addr flag(? are this must be here)
                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
                }
            }

            /**
             * Recv data from bus (async)
             * - check if state = READY
             * - prepare internal rx buffer
             * - enable rx interrupts + handler
             * - set state = BUSY_RX
             *
             * @param data
             * @param size
             * @param cb Event/error callback
             */
            void recv(uint8_t *data, uint16_t size, void (*cb)(void))
            {
                // master: check for select state, set buffer to read, send start, enable irq
                if (_state == State::READY)
                {
                    while (_regs->SR2 & I2C_SR2_BUSY)
                        ;

                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 &= ~I2C_CR1_POS;

                    _errors = static_cast<uint8_t>(Error::NONE);
                    _state = State::BUSY_RX;

                    _dataBuf = data;
                    _dataLen = size;
                    _dataCnt = size;

                    _regs->CR1 |= I2C_CR1_ACK;
                    _regs->CR1 |= I2C_CR1_START;
                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
                }

                // slave: check for listen state, set buffer to read, enable irq, set state to busy rx listen
                if (_state == State::LISTEN)
                {
                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 &= ~I2C_CR1_POS;

                    _errors = static_cast<uint8_t>(Error::NONE);
                    _state = State::BUSY_RX_LISTEN;

                    _dataBuf = data;
                    _dataLen = size;
                    _dataCnt = size;

                    // TODO clear addr flag(? are this must be here)
                    _regs->CR1 |= I2C_CR1_ACK;
                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
                }
            }

            // TODO mem flow used TX and TX_RX, not just RX, need detect somehow it in flow
            /**
             * Send data to slave memory like device (async)
             *
             * @param address
             * @param data
             * @param size
             * @param cb Event/error callback
             */
            template <typename T>
            void memSend(T address, uint8_t *data, uint16_t size, void (*cb)(void))
            {
                static_assert(std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t>, "Allowed only 8 and 16 bit address");

                if (_state == State::READY)
                {
                    while (_regs->SR2 & I2C_SR2_BUSY)
                        ;

                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 &= ~I2C_CR1_POS;

                    _errors = static_cast<uint8_t>(Error::NONE);
                    _state = State::BUSY_TX;

                    _addrVal = static_cast<T>(address);
                    _addrLen = sizeof(T);
                    _dataBuf = data;
                    _dataLen = size;
                    _dataCnt = size;

                    _regs->CR1 |= I2C_CR1_START;
                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
                }
            }

            /**
             * Recv data from slave memory like device (async)
             *
             * @param address
             * @param data
             * @param size
             * @param cb Event/error callback
             */
            template <typename T>
            void memRecv(T address, uint8_t *data, uint16_t size, void (*cb)(void))
            {
                static_assert(std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t>, "Allowed only 8 and 16 bit address");

                if (_state == State::READY)
                {
                    while (_regs->SR2 & I2C_SR2_BUSY)
                        ;

                    _regs->CR1 |= I2C_CR1_PE;
                    _regs->CR1 &= ~I2C_CR1_POS;

                    _errors = static_cast<uint8_t>(Error::NONE);
                    _state = State::BUSY_RX;

                    _addrVal = static_cast<T>(address);
                    _addrLen = sizeof(T);
                    _dataBuf = data;
                    _dataLen = size;
                    _dataCnt = size;

                    _regs->CR1 |= I2C_CR1_ACK;
                    _regs->CR1 |= I2C_CR1_START;
                    _regs->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN;
                }
            }

            /**
             * Abort any ongoing operation
             * - soft stop any operation
             * - disable tx/rx interrupts
             */
            void abort()
            {
                if (_regs->SR2 & I2C_SR2_BUSY && _devAddress)
                {
                    _state = State::ABORT;

                    _dataLen = 0;

                    _regs->CR1 &= ~I2C_CR1_ACK;
                    _regs->CR1 |= I2C_CR1_STOP;
                    _regs->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);

                    // TODO call user callback
                }
            }

            void _clearADDR()
            {
                (void)_regs->SR1;
                (void)_regs->SR2;
            }

            void _clearSTOPF()
            {
                (void)_regs->SR1;
                _regs->CR1 |= I2C_CR1_PE;
            }

            void dispatchEventIRQ()
            {
                uint32_t SR1 = _regs->SR1;
                if (_devAddress)
                {
                    // SB = 1; EV5
                    if ((SR1 & I2C_SR1_SB) == I2C_SR1_SB)
                    {
                        if (_state == State::BUSY_TX)
                            _regs->DR = _devAddress & ~0x01U;
                        else
                            _regs->DR = _devAddress | 0x01U;
                    }
                    // ADDR = 1: EV6
                    if ((SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR)
                    {
                        _clearADDR();
                        if (_state == State::BUSY_TX)
                        {
                            _regs->DR = *_dataBuf;
                            _dataBuf++;
                            _dataCnt--;
                            if (_dataCnt == 0)
                            {
                                _regs->CR2 &= ~I2C_CR2_ITBUFEN;
                            }
                        }
                        else
                        {
                            if (_dataCnt == 1)
                            {
                                _regs->CR1 &= ~I2C_CR1_ACK;
                                _regs->CR1 |= I2C_CR1_STOP;
                            }
                        }
                        // TODO what to do depends on simple rx/tx or mem rx/tx
                    }
                    // TXE = 1 & BTF = 0: EV8_1
                    if ((SR1 & (I2C_SR1_TXE | I2C_SR1_BTF)) == I2C_SR1_TXE)
                    {
                        if (_dataCnt != 0) {
                            _regs->DR = *_dataBuf;
                            _dataBuf++;
                            _dataCnt--;
                            if (_dataCnt == 0)
                            {
                                _regs->CR2 &= ~I2C_CR2_ITBUFEN;
                            }
                        }
                    }
                    // RXNE = 1: EV7
                    if ((SR1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) == I2C_SR1_RXNE)
                    {
                        if (_dataCnt != 0) {
                            *_dataBuf = _regs->DR;
                            _dataBuf++;
                            _dataCnt--;
                            if (_dataCnt == 1)
                            {
                                _regs->CR1 &= ~I2C_CR1_ACK;
                                _regs->CR1 |= ~I2C_CR1_STOP;
                            }
                        }
                    }
                }
                else
                {
                    // ADDR = 1: EV1
                    if ((SR1 & I2C_SR1_ADDR) == I2C_SR1_ADDR)
                    {
                        _clearADDR();
                        Direction dir = Direction::RX;
                        if (_regs->SR2 & I2C_SR2_TRA)
                            dir = Direction::TX;

                        (void)dir;
                        // TODO inform upper code
                    }
                    // STOPF = 1: EV4 (stopped by master)
                    else if (SR1 & I2C_SR1_STOPF)
                    {
                        _clearSTOPF();
                        // Disable ACK (?)
                        _regs->CR1 &= ~I2C_CR1_ACK;
                        // Disable IRQ
                        _regs->CR2 &= ~(I2C_CR2_ITEVTEN | I2C_CR2_ITERREN | I2C_CR2_ITBUFEN);
                    }
                    // RXNE = 1: EV2
                    else if ((SR1 & (I2C_SR1_RXNE | I2C_SR1_BTF)) == I2C_SR1_RXNE && _dataCnt != 0)
                    {
                        *_dataBuf = static_cast<uint8_t>(_regs->DR);
                        _dataBuf++;
                        _dataCnt--;
                        if (_dataCnt == 0)
                        {
                            _regs->CR2 &= ~I2C_CR2_ITBUFEN;
                            _state = State::LISTEN;
                            // TODO inform upper code
                        }
                    }
                    // TXE = 1: EV3
                    else if ((SR1 & (I2C_SR1_TXE | I2C_SR1_BTF)) == I2C_SR1_TXE && _dataCnt != 0)
                    {
                        _regs->DR = *_dataBuf;
                        _dataBuf++;
                        _dataCnt--;
                        if (_dataCnt == 0)
                        {
                            _regs->CR2 &= ~I2C_CR2_ITBUFEN;
                            _state = State::LISTEN;
                            // TODO inform upper code
                        }
                    }
                    else if ((SR1 & I2C_SR1_BTF) == I2C_SR1_BTF && _dataCnt != 0)
                    {
                        if (_state == State::BUSY_TX_LISTEN)
                            _regs->DR = *_dataBuf;
                        else
                            *_dataBuf = static_cast<uint8_t>(_regs->DR);
                        _dataBuf++;
                        _dataCnt--;
                    }
                }
            }

            /**
             * Dispatch interrupt
             * - dispatch error interrupts
             * - dispatch event interrupts (selected only)
             */
            void dispatchErrorIRQ()
            {
                uint8_t errors = static_cast<uint8_t>(Error::NONE);
                if (_regs->SR1 & I2C_SR1_BERR)
                {
                    errors |= static_cast<uint8_t>(Error::BERR);
                    _regs->SR1 &= ~I2C_SR1_BERR;
                    _regs->CR1 |= I2C_CR1_SWRST;
                }

                if (_regs->SR1 & I2C_SR1_ARLO)
                {
                    errors |= static_cast<uint8_t>(Error::ARLO);
                    _regs->SR1 &= ~I2C_SR1_ARLO;
                }

                if (_regs->SR1 & I2C_SR1_AF)
                {
                    errors |= static_cast<uint8_t>(Error::AF);
                    _regs->SR1 &= ~I2C_SR1_AF;
                    if (_devAddress)
                        _regs->CR1 |= I2C_CR1_STOP;
                }

                if (_regs->SR1 & I2C_SR1_OVR)
                {
                    errors |= static_cast<uint8_t>(Error::OVR);
                    _regs->SR1 &= ~I2C_SR1_OVR;
                }

                if (errors != static_cast<uint8_t>(Error::NONE))
                {
                    _errors = errors;
                    // TODO user callback
                }
            }
        };
    }

// Aliases for all instances
#if defined(I2C1_BASE)
    using I2C1_Driver = I2C::Driver<I2C1_BASE, I2C1_EV_IRQn, I2C1_ER_IRQn>;
#endif
#if defined(I2C2_BASE)
    using I2C2_Driver = I2C::Driver<I2C2_BASE, I2C2_EV_IRQn, I2C2_ER_IRQn>;
#endif
#if defined(I2C3_BASE)
    using I2C3_Driver = I2C::Driver<I2C3_BASE, I2C3_EV_IRQn, I2C3_ER_IRQn>;
#endif
#if defined(I2C4_BASE)
    using I2C4_Driver = I2C::Driver<I2C4_BASE, I2C4_EV_IRQn, I2C4_ER_IRQn>;
#endif
#if defined(I2C5_BASE)
    using I2C5_Driver = I2C::Driver<I2C5_BASE, I2C5_EV_IRQn, I2C5_ER_IRQn>;
#endif
#if defined(I2C6_BASE)
    using I2C6_Driver = I2C::Driver<I2C6_BASE, I2C6_EV_IRQn, I2C6_ER_IRQn>;
#endif
}

#endif // __STM32_I2C_HPP__