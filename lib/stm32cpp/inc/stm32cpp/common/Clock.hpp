#ifndef __STM32_CLOCK_COMMON_HPP__
#define __STM32_CLOCK_COMMON_HPP__

#include <stdint.h>
#include <type_traits>
#include <stm32cpp/_common.hpp>

namespace STM32
{
    namespace Clock
    {
        class ClockBase
        {
        protected:
            template <volatile uint32_t RCC_TypeDef::* reg, uint32_t tTurnMask, uint32_t tWaitMask>
            static inline void _enable()
            {
                reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg |= tTurnMask;
                if constexpr (tWaitMask != 0u) {
                    while ((reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg & tWaitMask) == 0u);
                }
            }
            template <volatile uint32_t RCC_TypeDef::* reg, uint32_t tTurnMask, uint32_t tWaitMask>
            static inline void _disable()
            {
                reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg &= ~tTurnMask;
                if constexpr (tWaitMask != 0u) {
                    while ((reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg & tWaitMask) == 0u);
                }
            }
        };

        template <volatile uint32_t RCC_TypeDef::* reg, uint32_t tTurnMask>
        class ClockControl
        {
        public:
            static inline void enable()
            {
                reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg |= tTurnMask;
            }
            static inline void disable()
            {
                reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg &= ~tTurnMask;
            }
        };

        template <volatile uint32_t RCC_TypeDef::* reg, uint32_t tTurnMask>
        class ResetControl
        {
        public:
            static inline void reset()
            {
                reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg |= tTurnMask;
                __DSB();
                reinterpret_cast<RCC_TypeDef*>(RCC_BASE)->*reg &= ~tTurnMask;
            }
        };

        class LSIClock : public ClockBase
        {
        public:
            static inline uint32_t getFrequency()
            {
                return LSI_VALUE;
            }
            static inline void on()
            {
                ClockBase::_enable<&RCC_TypeDef::CSR, RCC_CSR_LSION, RCC_CSR_LSIRDY>();
            }
            static inline void off()
            {
                ClockBase::_disable<&RCC_TypeDef::CSR, RCC_CSR_LSION, RCC_CSR_LSIRDY>();
            }
        };

        class LSEClock : public ClockBase
        {
        public:
            static inline uint32_t getFrequency()
            {
                return LSE_VALUE;
            }
            static inline void on()
            {
#if defined(STM32F0)
                ClockBase::_enable<&RCC_TypeDef::CSR, RCC_CSR_LSEON, RCC_CSR_LSERDY>();
#else
                ClockBase::_enable<&RCC_TypeDef::BDCR, RCC_BDCR_LSEON, RCC_BDCR_LSERDY>();
#endif
            }
            static inline void off()
            {
#if defined(STM32F0)
                ClockBase::_disable<&RCC_TypeDef::CSR, RCC_CSR_LSEON, RCC_CSR_LSERDY>();
#else
                ClockBase::_disable<&RCC_TypeDef::BDCR, RCC_BDCR_LSEON, RCC_BDCR_LSERDY>();
#endif
            }
        };

#if defined(STM32L1) || defined(STM32L4) || defined(STM32L5)
        enum class MSI_Range
        {
            _100_kHz,
            _200_kHz,
            _400_kHz,
            _800_kHz,
            _1M_Hz,
            _2_MHz,
            _4_MHz, //<- reset value
            _8_MHz,
            _16_MHz,
            _24_MHz,
            _32_MHz,
            _48_MHz,
        };
        class MSIClock : public ClockBase
        {
            static inline uint32_t getFrequency()
            {
                return MSI_VALUE;
            }
            static inline void on()
            {
                ClockBase::_enable<&RCC_TypeDef::CR, RCC_CR_MSION, RCC_CR_MSIRDY>();
            }
            static inline void off()
            {
                ClockBase::_disable<&RCC_TypeDef::CR, RCC_CR_MSION, RCC_CR_MSIRDY>();
            }
            template <MSI_Range tRange>
            static inline void configure() // TODO config struct(?)
            {
                RCC->CR = (RCC->CR & ~RCC_CR_MSIRANGE) | (static_cast<uint8_t>(tRange) << RCC_CR_MSIRANGE_Pos);
            }
        };
#endif

        class HSIClock : public ClockBase
        {
        public:
            static inline uint32_t getFrequency()
            {
                return HSI_VALUE;
            }
            static inline void on()
            {
                ClockBase::_enable<&RCC_TypeDef::CR, RCC_CR_HSION, RCC_CR_HSIRDY>();
            }
            static inline void off()
            {
                ClockBase::_disable<&RCC_TypeDef::CR, RCC_CR_HSION, RCC_CR_HSIRDY>();
            }
            template <uint32_t tTrim>
            static inline void configure() // TODO config struct(?)
            {
                static_assert(tTrim < 32);
                RCC->CR = (RCC->CR & ~RCC_CR_HSITRIM) | (tTrim << RCC_CR_HSITRIM_Pos);
            }
        };

#if defined(RCC_HSI48_SUPPORT)
        class HSI48Clock : public ClockBase
        {
        public:
            static inline uint32_t getFrequency()
            {
                return HSI48_VALUE;
            }
            static inline void on()
            {
                ClockBase::_enable<&RCC_TypeDef::CR, RCC_CR_HSI48ON, RCC_CR_HSI48RDY>();
            }
            static inline void off()
            {
                ClockBase::_disable<&RCC_TypeDef::CR, RCC_CR_HSI48ON, RCC_CR_HSI48RDY>();
            }
        };
#endif

        class HSEClock : public ClockBase
        {
        public:
            static inline uint32_t getFrequency()
            {
                return HSE_VALUE;
            }
            static inline void on()
            {
                ClockBase::_enable<&RCC_TypeDef::CR, RCC_CR_HSEON, RCC_CR_HSERDY>();
            }
            static inline void off()
            {
                ClockBase::_disable<&RCC_TypeDef::CR, RCC_CR_HSEON, RCC_CR_HSERDY>();
            }
        };

        enum class PLLClockSource
        {
#if defined(STM32G0) || defined(STM32G4) || defined(STM32L4) || defined(STM32L5)
            NONE,
#endif
#if defined(STM32L4) || defined(STM32L5)
            MSI,
#endif
            HSI,
            HSE,
        };

        class PLLClock : public ClockBase
        {
        private:
            template <typename T>
            static inline void _update();

            template <typename T>
            static inline void _configure();

        public:
            static inline uint32_t getFrequency();

            static inline void on()
            {
                ClockBase::_enable<&RCC_TypeDef::CR, RCC_CR_PLLON, RCC_CR_PLLRDY>();
            }

            static inline void off()
            {
                ClockBase::_disable<&RCC_TypeDef::CR, RCC_CR_PLLON, RCC_CR_PLLRDY>();
            }

            // CFGR register for F0, F1, L0, PLLCFGR register for F2, F3, F4, F7, L4, G0, G4
            template <typename T>
            static inline void configure()
            {
                _configure<T>();
                _update<T>();
            }
        };

        enum class SYSClockSource
        {
#if defined(STM32L0)
            MSI = RCC_CFGR_SW_MSI,
#endif
            HSI = RCC_CFGR_SW_HSI, // G0: config divider
            HSE = RCC_CFGR_SW_HSE,
            PLL = RCC_CFGR_SW_PLL,
#if defined(STM32G0)
            LSI = RCC_CFGR_SW_LSI,
            LSE = RCC_CFGR_SW_LSE,
#endif
        };

        class SYSClock
        {
        private:
            template <SYSClockSource tSource>
            static inline void _update()
            {
#if defined(STM32L0)
                if constexpr (tSource == SYSClockSource::MSI) {
                    SystemCoreClock = MSIClock::getFrequency();
                }
#endif
                if constexpr (tSource == SYSClockSource::HSI) {
                    SystemCoreClock = HSIClock::getFrequency();
                }
                if constexpr (tSource == SYSClockSource::HSE) {
                    SystemCoreClock = HSEClock::getFrequency();
                }
                if constexpr (tSource == SYSClockSource::PLL) {
                    SystemCoreClock = PLLClock::getFrequency();
                }
#if defined(STM32G0)
                if constexpr (tSource == SYSClockSource::LSI) {
                    SystemCoreClock = LSIClock::getFrequency();
                }
                if constexpr (tSource == SYSClockSource::LSE) {
                    SystemCoreClock = LSEClock::getFrequency();
                }
#endif
                SystemCoreClock >>= AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
            }

            template <typename T>
            static inline void _configure();

        public:
            static inline uint32_t getFrequency()
            {
                return SystemCoreClock;
            }

            // L0: hpre(AHB), ppre1(APB1), ppre2(APB2), usb clocked by hsi48/dedicated pll out(if has usb)
            // L4: hpre(AHB), ppre1(APB1), ppre2(APB2), clk48sel(USB)
            // F0: hpre(AHB), ppre(APB)
            // F1: hpre(AHB), ppre1(APB1), ppre2(APB2), adcpre, usbpre
            // F2, F4, F7, G4: hpre(AHB), ppre1(APB1), ppre2(APB2), usb clocked by pllQ line
            // F3: hpre(AHB), ppre1(APB1), ppre2(APB2), adc12pres, adc34pres, usbpre
            // G0: hpre(AHB), ppre(APB), usb clocked by hsi48/hse/pllQ line(if has usb)
            template <typename T>
            static inline void configure()
            {
                _configure<T>();
                _update<T::Source>();
            }
        };

        enum class RTCClockSource
        {
            NONE,
#if defined(STM32L0)
            LSE = RCC_CSR_RTCSEL_LSE,
            LSI = RCC_CSR_RTCSEL_LSI,
            HSE = RCC_CSR_RTCSEL_HSE,
#else
            LSE = RCC_BDCR_RTCSEL_LSE,
            LSI = RCC_BDCR_RTCSEL_LSI,
            HSE = RCC_BDCR_RTCSEL_HSE,
#endif
        };

        class RTCClock
        {
        public:
            // HSE: L0 - no div; F0, G0, G4, L4 - div by 32; F1 - div by 128, F2,3,4, 7 - div by config
            static inline uint32_t getFrequency();

            static inline void enable()
            {
#if defined(STM32L0)
                RCC->CSR |= RCC_CSR_RTCEN;
#else
                RCC->BDCR |= RCC_BDCR_RTCEN;
#endif
            }

            static inline void disable()
            {
#if defined(STM32L0)
                RCC->CSR &= ~RCC_CSR_RTCEN;
#else
                RCC->BDCR &= ~RCC_BDCR_RTCEN;
#endif
            }

            template <typename T>
            static inline void configure();
        };
    }
}

#endif // __STM32_CLOCK_COMMON_HPP__
