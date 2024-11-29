#ifndef __STM32_SPI_HPP__
#define __STM32_SPI_HPP__

#include <stdint.h>
#include "_common.hpp"
#include <stm32cpp/Clock.hpp>
#include <stm32cpp/IO.hpp>

extern "C"
{
#include <stm32f1xx_hal_spi.h>
}

#include <stm32cpp/SPI_definitions.hpp>

namespace STM32
{
    namespace SPI
    {
        enum class N
        {
#if defined(SPI1_BASE)
            _1,
#endif
#if defined(SPI2_BASE)
            _2,
#endif
#if defined(SPI3_BASE)
            _3,
#endif
#if defined(SPI4_BASE)
            _4,
#endif
#if defined(SPI5_BASE)
            _5,
#endif
#if defined(SPI6_BASE)
            _6,
#endif
            TOTAL,
        };

        static_assert(static_cast<uint8_t>(N::TOTAL) != 0U, "Device doesn't support SPI");

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

        class Config
        {
        private:
            uint32_t _clock;
            BitOrder _bitOrder;
            BusMode _busMode;
            BusLine _busLine;

        public:
            constexpr Config()
                : _clock(1000000),
                  _bitOrder(BitOrder::MSB),
                  _busMode(BusMode::MODE_0),
                  _busLine(BusLine::TWO_LINE) {};

            constexpr Config(uint32_t clk, BitOrder bo, BusMode bm, BusLine bl)
                : _clock(clk), _bitOrder(bo), _busMode(bm), _busLine(bl) {}
        };

        enum class Signal
        {
            NONE
        };

        struct Data
        {
            uint8_t *buf;
            size_t len;
            size_t cnt;
        };

        template <uint32_t regs_base, IRQn_Type eventIRQn, class TClock, class TReset>
        class Driver
        {
        private:
            static constexpr SPI_TypeDef *_regs() { return reinterpret_cast<SPI_TypeDef *>(regs_base); };

            Driver() {}

            Driver(const Driver &) = delete;         // Prevent copy constructor
            Driver(Driver &&) = delete;              // Prevent move constructor
            void operator=(const Driver &) = delete; // Prevent assignment
            void operator=(Driver &&) = delete;      // Prevent reference

            static Data _txData;
            static Data _rxData;

        public:
            /**
             * Construct a new Driver object singleton
             */
            static Driver &instance()
            {
                static Driver instance;
                return instance;
            }

            void init()
            {
                TClock::enable();
                TReset::reset();

                NVIC_ClearPendingIRQ(eventIRQn);
                NVIC_EnableIRQ(eventIRQn);
            }

            void deinit()
            {
                TClock::disable();
                NVIC_DisableIRQ(eventIRQn);
            }

            /**
             * Lister to CS pin, automatically go to slave mode
             * - configure CS for listen
             * - configure bus options
             * - enable interrupts
             *
             * @param pin
             * @param config
             */
            void listen(void *pin, const Config config)
            {
                // IO::PA10::attachIRQ<EXTIEdge::FALLING>();//TODO dev selected
                // IO::PA10::attachIRQ<EXTIEdge::RISING>();//TODO dev unselected
            }

            /**
             * Select slave device, automatically go to master mode
             * - configure CS fo use in operations(?)
             * - configure bus options
             *
             * @param pin    Chip select helper
             * @param config Bus config for selected device if any
             */
            void select(void *pin, const Config config) {}

            /**
             * Send data to bus (async)
             * - prepare internal tx buffer
             * - enable tx interrupts + handler
             *
             * @param data
             * @param size
             * @param cb Event/error callback
             */
            void send(uint8_t *data, uint16_t size, void (*cb)(void)) {}

            /**
             * Recv data from bus (async)
             * - prepare internal rx buffer
             * - enable rx interrupts + handler
             *
             * @param data
             * @param size
             * @param cb Event/error callback
             */
            void recv(uint8_t *data, uint16_t size, void (*cb)(void)) {}

            /**
             * Send and recv data to/from bus at once
             * - prepare internal tx/rx buffers
             * - enable tx/rx interrupts + handler
             *
             * @param txData
             * @param rxData
             * @param size
             * @param cb Event/error callback
             */
            /////exchange?
            void transfer(uint8_t *txData, uint8_t *rxData, uint16_t size, void (*cb)(void)) {}

            /**
             * Abort any ongoing operation
             * - soft stop any operation
             * - disable tx/rx interrupts
             */
            void abort() {}

            /**
             * Dispatch interrupt
             * - dispatch error interrupts
             * - dispatch event interrupts (selected only)
             */
            void dispatchIRQ()
            {
                uint32_t CR2 = _regs()->CR2;
                uint32_t SR = _regs()->SR;

                if ((SR & SPI_SR_OVR) == 0u && (SR & SPI_SR_RXNE) != 0u && (CR2 & SPI_CR2_RXNEIE) != 0u)
                {
                    *_rxData.buf = _regs()->DR;

                    _rxData.buf++;
                    _rxData.cnt++;
                    _rxData.len--;

                    if (_rxData.len == 0u)
                    {
                        // TODO done rx
                    }
                    return;
                }
                if ((SR & SPI_SR_TXE) != 0u && (CR2 & SPI_CR2_TXEIE) != 0u)
                {
                    _regs()->DR = *_txData.buf;

                    _txData.buf++;
                    _txData.cnt++;
                    _txData.len--;

                    if (_txData.len == 0u)
                    {
                        // TODO done tx
                    }
                    return;
                }
                if (((SR & SPI_SR_MODF) != 0u || (SR & SPI_SR_OVR) != 0u) && (CR2 & SPI_CR2_ERRIE) != 0u)
                {
                    if ((SR & SPI_SR_OVR) != 0u)
                        _clearOVR();

                    if ((SR & SPI_SR_MODF) != 0u)
                        _clearMODF();

                    _regs()->CR2 &= ~(SPI_CR2_ERRIE | SPI_CR2_RXNEIE | SPI_CR2_TXEIE);
                    // TODO call cb
                    //_event = ERROR;// enum class Event{NONE, TX_DONE, RX_DONE, TX_RX_DONE, ERROR}
                }
            }

            static inline void _clearOVR()
            {
                __IO uint32_t tmp = 0x00U;
                tmp = _regs()->DR;
                tmp = _regs()->SR;
                (void)(tmp);
            }

            static inline void _clearMODF()
            {
                __IO uint32_t tmp = 0x00U;
                tmp = _regs()->SR;
                CLEAR_BIT(_regs()->CR1, SPI_CR1_SPE);
                (void)(tmp);
            }
        };
    }

// Aliases for all instances
#if defined(SPI1_BASE)
    using SPI1_Driver = SPI::Driver<
        SPI1_BASE,
        SPI1_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_SPI1EN>,
        Clock::ResetControl<&RCC_TypeDef::APB2RSTR, RCC_APB2RSTR_SPI1RST>>;
#endif
#if defined(SPI2_BASE)
    using SPI2_Driver = SPI::Driver<
        SPI2_BASE,
        SPI2_IRQn,
        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_SPI2EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_SPI2RST>>;
#endif
#if defined(SPI3_BASE)
    using SPI3_Driver = SPI::Driver<
        SPI3_BASE,
        SPI3_IRQn,

        Clock::ClockControl<&RCC_TypeDef::APB1ENR, RCC_APB1ENR_SPI3EN>,
        Clock::ResetControl<&RCC_TypeDef::APB1RSTR, RCC_APB1RSTR_SPI3RST>>;
#endif
#if defined(SPI4_BASE)
    using SPI4_Driver = SPI::Driver<
        SPI4_BASE,
        SPI4_IRQn,

        Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_SPI4EN>,
        Clock::ResetControl<&RCC_TypeDef::APB2RSTR, RCC_APB2RSTR_SPI4RST>>;
#endif
#if defined(SPI5_BASE)
    using SPI5_Driver = SPI::Driver<
        SPI5_BASE,
        SPI5_IRQn,

        Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_SPI5EN>,
        Clock::ResetControl<&RCC_TypeDef::APB2RSTR, RCC_APB2RSTR_SPI5RST>>;
#endif
#if defined(SPI6_BASE)
    using SPI6_Driver = SPI::Driver<
        SPI6_BASE,
        SPI6_IRQn,

        Clock::ClockControl<&RCC_TypeDef::APB2ENR, RCC_APB2ENR_SPI6EN>,
        Clock::ResetControl<&RCC_TypeDef::APB2RSTR, RCC_APB2RSTR_SPI6RST>>;
#endif
}

#endif // __STM32_SPI_HPP__