#ifndef __STM32_DMA_HPP__
#define __STM32_DMA_HPP__

#include <stdint.h>
#include <stm32cpp/_common.hpp>
#include <stm32cpp/Clock.hpp>

// F0, F1, F3, G0, G4, L1, L4 - channel; IRQn {GLOBAL, TC, HT, TE}
// F2, F4, F7 - stream; IRQn {FIFO, reserved, DME, TE, HT, TC}
namespace STM32
{
    namespace DMA
    {
        enum class Config
        {
            // Direction
            PER_2_MEM = 0x00000000u,
            MEM_2_PER = DMA_CCR_DIR,
            MEM_2_MEM = DMA_CCR_MEM2MEM,
            // Circular mode
            CIRCULAR = DMA_CCR_CIRC,
            // Increments
            PINC = DMA_CCR_PINC,
            MINC = DMA_CCR_MINC,
            // Periph data size
            PSIZE_8BIT = 0x00000000u,
            PSIZE_16BIT = DMA_CCR_PSIZE_0,
            PSIZE_32BIT = DMA_CCR_PSIZE_1,
            // Memory data size
            MSIZE_8BIT = 0x00000000u,
            MSIZE_16BIT = DMA_CCR_MSIZE_0,
            MSIZE_32BIT = DMA_CCR_MSIZE_1,
            // Priority
            PRIORITY_LOW = 0x00000000u,
            PRIORITY_MEDIUM = DMA_CCR_PL_0,
            PRIORITY_HIGH = DMA_CCR_PL_1,
            PRIORITY_VERY_HIGH = DMA_CCR_PL,
        };

        inline Config operator|(Config lft, Config rgt)
        {
            return Config(static_cast<uint32_t>(lft) | static_cast<uint32_t>(rgt));
        }

        enum class Flag
        {
            GLOBAL,
            TRANSFER_COMPLETE,
            HALF_TRANSFER,
            ERROR,
        };

        using TransferCallbackT = std::add_pointer_t<void()>;// TODO use from common
        using ErrorCallbackT = std::add_pointer_t<void()>;

        template <class tDriver, uint32_t tRegsAddress, IRQn_Type tEventIRQn, uint8_t tChannel>
        class Channel
        {
        private:
            static TransferCallbackT _transferCallback;
            static ErrorCallbackT _errorCallback;

            static constexpr DMA_Channel_TypeDef *_regs()
            {
                return reinterpret_cast<DMA_Channel_TypeDef *>(tRegsAddress);
            }

            template <Flag tFlag>
            static inline bool _getFlag()
            {
                return tDriver::template getChannelFlag<tChannel, tFlag>();
            }

            template <Flag tFlag>
            static inline void _clrFlag()
            {
                tDriver::template clrChannelFlag<tChannel, tFlag>();
            }

            static inline void _clrFlags()
            {
                tDriver::template clrChannelFlags<tChannel>();
            }

        public:
            static inline void setTransferCallback(TransferCallbackT cb)
            {
                _transferCallback = cb;
            }

            static inline void setErrorCallback(ErrorCallbackT cb)
            {
                _errorCallback = cb;
            }

            template <typename tConfig>
            static inline void transfer(const void *buffer, volatile void *periph, uint16_t len)
            {
                static constexpr auto config = static_cast<uint32_t>(tConfig);

                tDriver::enable();

                _regs()->CCR = 0x00000000u;

                _regs()->CNDTR = len;
                _regs()->CMAR = reinterpret_cast<uint32_t>(buffer);
                _regs()->CPAR = reinterpret_cast<uint32_t>(periph);

                NVIC_EnableIRQ(tEventIRQn);
                _regs()->CCR = config | DMA_CCR_EN | DMA_CCR_TEIE | DMA_CCR_TCIE;
            }

            static inline uint16_t getRemaining()
            {
                return _regs()->CNDTR;
            }

            static inline void abort()
            {
                _regs()->CCR &= ~(DMA_CCR_TEIE | DMA_CCR_HTIE | DMA_CCR_TCIE);
                _regs()->CCR &= ~(DMA_CCR_EN);
                _clrFlags();
            }

            static inline bool isReady()
            {
                return _regs()->CNDTR == 0 || (_regs()->CCR & DMA_CCR_EN) == 0u;
            }

            static inline void dispatchIRQ()
            {
                if ((_regs()->CCR & DMA_CCR_TCIE) != 0u && _getFlag<Flag::TRANSFER_COMPLETE>())
                {
                    _clrFlag<Flag::TRANSFER_COMPLETE>();
                    if (_transferCallback)
                        _transferCallback();
                }

                if ((_regs()->CCR & DMA_CCR_TEIE) != 0u && _getFlag<Flag::ERROR>())
                {
                    _regs()->CCR &= ~(DMA_CCR_TEIE | DMA_CCR_HTIE | DMA_CCR_TCIE);
                    _clrFlags();
                    if (_errorCallback)
                        _errorCallback();
                }
            }
        };

        template <uint32_t tRegsAddress, class tClock>
        class Driver
        {
        private:
            static constexpr DMA_TypeDef *_regs() { return reinterpret_cast<DMA_TypeDef *>(tRegsAddress); }

        public:
            static inline void enable()
            {
                tClock::enable();
            }

            static inline void disable()
            {
                tClock::disable();
            }

            template <uint8_t tChannel, Flag tFlag>
            static inline bool getChannelFlag()
            {
                return _regs()->ISR & (1 << ((tChannel - 1) * 4 + static_cast<uint32_t>(tFlag)));
            }

            template <uint8_t tChannel, Flag tFlag>
            static inline void clrChannelFlag()
            {
                _regs()->IFCR |= (1 << ((tChannel - 1) * 4) + static_cast<uint32_t>(tFlag));
            }

            template <uint8_t tChannel>
            static inline void clrChannelFlags()
            {
                _regs()->IFCR |= (0x0Fu << ((tChannel - 1) * 4));
            }
        };
    }

// F10x only for now
#if defined(DMA1_BASE)
#undef DMA1
    using DMA1 = DMA::Driver<DMA1_BASE, Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_DMA1EN>>;

#undef DMA1_Channel1
#undef DMA1_Channel2
#undef DMA1_Channel3
#undef DMA1_Channel4
#undef DMA1_Channel5
#undef DMA1_Channel6
#undef DMA1_Channel7
    using DMA1_Channel1 = DMA::Channel<DMA1, DMA1_Channel1_BASE, DMA1_Channel1_IRQn, 1>;
    using DMA1_Channel2 = DMA::Channel<DMA1, DMA1_Channel2_BASE, DMA1_Channel2_IRQn, 2>;
    using DMA1_Channel3 = DMA::Channel<DMA1, DMA1_Channel3_BASE, DMA1_Channel3_IRQn, 3>;
    using DMA1_Channel4 = DMA::Channel<DMA1, DMA1_Channel4_BASE, DMA1_Channel4_IRQn, 4>;
    using DMA1_Channel5 = DMA::Channel<DMA1, DMA1_Channel5_BASE, DMA1_Channel5_IRQn, 5>;
    using DMA1_Channel6 = DMA::Channel<DMA1, DMA1_Channel6_BASE, DMA1_Channel6_IRQn, 6>;
    using DMA1_Channel7 = DMA::Channel<DMA1, DMA1_Channel7_BASE, DMA1_Channel7_IRQn, 7>;
#endif
#if defined(DMA2_BASE)
#undef DMA2
    using DMA2 = DMA::Driver<DMA2_BASE, Clock::ClockControl<&RCC_TypeDef::AHBENR, RCC_AHBENR_DMA2EN>>;

#undef DMA2_Channel1
#undef DMA2_Channel2
#undef DMA2_Channel3
#undef DMA2_Channel4
#undef DMA2_Channel5
    using DMA2_Channel1 = DMA::Channel<DMA2, DMA2_Channel1_BASE, DMA2_Channel1_IRQn, 1>;
    using DMA2_Channel2 = DMA::Channel<DMA2, DMA2_Channel2_BASE, DMA2_Channel2_IRQn, 2>;
    using DMA2_Channel3 = DMA::Channel<DMA2, DMA2_Channel3_BASE, DMA2_Channel3_IRQn, 3>;
    using DMA2_Channel4 = DMA::Channel<DMA2, DMA2_Channel4_BASE, DMA2_Channel4_IRQn, 4>;
    using DMA2_Channel5 = DMA::Channel<DMA2, DMA2_Channel5_BASE, DMA2_Channel5_IRQn, 5>;
#endif
}

#endif // __STM32_DMA_HPP__
