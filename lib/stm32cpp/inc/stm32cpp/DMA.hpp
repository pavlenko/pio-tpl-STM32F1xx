#ifndef __STM32_DMA_HPP__
#define __STM32_DMA_HPP__

#include <stdint.h>
#include <stm32cpp/_common.hpp>
#include <stm32cpp/Clock.hpp>
#include <stm32cpp/common/DMA_definitions.hpp>

// F0, F1, F3, G0, G4, L1, L4 - channel; IRQn {GLOBAL, TC, HT, TE}
// F2, F4, F7 - stream; IRQn {FIFO, reserved, DME, TE, HT, TC}
namespace STM32
{
    namespace DMA
    {
        using ErrorCallbackT = std::add_pointer_t<void()>;

        template <class tDriver, uint32_t tRegsAddress, IRQn_Type tEventIRQn, uint8_t tChannel>
        class Channel
        {
        private:
            static inline Data data;
            static constexpr DMA_Channel_TypeDef *_regs()
            {
                return reinterpret_cast<DMA_Channel_TypeDef *>(tRegsAddress);
            }

        public:
            static inline void enable()
            {
#ifdef DMA_CCR_EN
                _regs()->CCR |= DMA_CCR_EN;
#endif
#ifdef DMA_SxCR_EN
                _regs()->CR |= DMA_SxCR_EN;
#endif
            }
            static inline void disable()
            {
#ifdef DMA_CCR_EN
                _regs()->CCR &= ~DMA_CCR_EN;
#endif
#ifdef DMA_SxCR_EN
                _regs()->CR &= ~DMA_SxCR_EN;
#endif
            }

            static inline void setTransferCallback(TransferCallback cb)
            {
                data.callback = cb;
            }

            static inline void transfer(Config config, const void *buffer, volatile void *periph, uint16_t len)
            {
                tDriver::enable();

#ifdef DMA_CCR_EN
                _regs()->CCR = 0;
                _regs()->CNDTR = len;
                _regs()->CMAR = reinterpret_cast<uint32_t>(buffer);
                _regs()->CPAR = reinterpret_cast<uint32_t>(periph);
#endif
#ifdef DMA_SxCR_EN
                _regs()->CR = 0;
                _regs()->NDTR = len;
                _regs()->M0AR = reinterpret_cast<uint32_t>(buffer);
                _regs()->PAR = reinterpret_cast<uint32_t>(periph);
#endif
                data.data = const_cast<void*>(buffer);
                data.size = len;

                if (data.callback)
                    config = config | Config::IRQ_TRANSFER_COMPLETE | Config::IRQ_TRANSFER_ERROR;

                NVIC_EnableIRQ(tEventIRQn);

#ifdef DMA_CCR_EN
                _regs()->CCR = static_cast<uint32_t>(config) | DMA_CCR_EN;
#endif
#ifdef DMA_SxCR_EN
                _regs()->CR = static_cast<uint32_t>(config) | DMA_SxCR_EN;
#endif
            }

            static inline uint16_t getRemaining()
            {
#ifdef DMA_CCR_EN
                return _regs()->CNDTR;
#endif
#ifdef DMA_SxCR_EN
                return _regs()->NDTR;
#endif
            }

            static inline void abort()
            {
#ifdef DMA_CCR_EN
                _regs()->CCR &= ~static_cast<uint32_t>(Config::IRQ_TRANSFER_COMPLETE | Config::IRQ_TRANSFER_ERROR);
#endif
#ifdef DMA_SxCR_EN
                _regs()->CR &= ~static_cast<uint32_t>(Config::IRQ_TRANSFER_COMPLETE | Config::IRQ_TRANSFER_ERROR);
#endif
                disable();
                clrFlags();
            }

            static inline bool isEnabled()
            {
#ifdef DMA_CCR_EN
                return (_regs()->CCR & DMA_CCR_EN) != 0u;
#endif
#ifdef DMA_SxCR_EN
                return (_regs()->CR & DMA_SxCR_EN) != 0u;
#endif
            }

            static inline bool isReady()
            {
                return getRemaining() == 0 || !isEnabled();
            }

            template <Flag tFlag>
            static inline bool getFlag()
            {
                return tDriver::template getChannelFlag<tChannel, tFlag>();
            }

            template <Flag tFlag>
            static inline void clrFlag()
            {
                tDriver::template clrChannelFlag<tChannel, tFlag>();
            }

            static inline void clrFlagTC()
            {
                tDriver::template clrChannelFlag<tChannel, Flag::TRANSFER_COMPLETE>();
            }

            static inline void clrFlags()
            {
                tDriver::template clrChannelFlags<tChannel>();
            }

            static inline void dispatchIRQ()
            {
                if (getFlag<Flag::TRANSFER_COMPLETE>())
                {
                    clrFlag<Flag::TRANSFER_COMPLETE>();
                    data.notify(true);//TODO <-- pass remaining counter, for check if full success...
                }

                if (getFlag<Flag::TRANSFER_ERROR>())
                {
                    clrFlags();
                    data.notify(false);
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
                _regs()->IFCR |= (1 << ((tChannel - 1) * 4 + static_cast<uint32_t>(tFlag)));
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
