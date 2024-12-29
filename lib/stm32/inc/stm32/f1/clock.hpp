#pragma once

#include <stm32/common/clock.hpp>

namespace STM32::Clock
{
    bool LSIClock::on() { ClockBase::enable<&RCC_TypeDef::CSR, RCC_CSR_LSION, RCC_CSR_LSIRDY>(); }

    bool LSIClock::off() { ClockBase::disable<&RCC_TypeDef::CSR, RCC_CSR_LSION, RCC_CSR_LSIRDY>(); }

    bool LSEClock::on() { ClockBase::enable<&RCC_TypeDef::BDCR, RCC_BDCR_LSEON, RCC_BDCR_LSERDY>(); }

    bool LSEClock::off() { ClockBase::disable<&RCC_TypeDef::BDCR, RCC_BDCR_LSEON, RCC_BDCR_LSERDY>(); }

    bool HSIClock::on() { ClockBase::enable<&RCC_TypeDef::CR, RCC_CR_HSION, RCC_CR_HSIRDY>(); }

    bool HSIClock::off() { ClockBase::disable<&RCC_TypeDef::CR, RCC_CR_HSION, RCC_CR_HSIRDY>(); }

    bool HSEClock::on() { ClockBase::enable<&RCC_TypeDef::CR, RCC_CR_HSEON, RCC_CR_HSERDY>(); }

    bool HSEClock::off() { ClockBase::disable<&RCC_TypeDef::CR, RCC_CR_HSEON, RCC_CR_HSERDY>(); }

    enum class PLLClock::Source
    {
        HSI,
        HSE,
    };

    bool PLLClock::on() { ClockBase::enable<&RCC_TypeDef::CR, RCC_CR_PLLON, RCC_CR_PLLRDY>(); }

    bool PLLClock::off() { ClockBase::disable<&RCC_TypeDef::CR, RCC_CR_PLLON, RCC_CR_PLLRDY>(); }

    template <PLLClock::Source source>
    void PLLClock::selectSource()
    {
        if constexpr (source == PLLClock::Source::HSE)
        {
            RCC->CFGR |= RCC_CFGR_PLLSRC;
        }
        else
        {
            RCC->CFGR &= ~RCC_CFGR_PLLSRC;
        }
    }

    template <uint32_t tDivider>
    void PLLClock::setDivider()
    {
#if defined(RCC_CFGR2_PREDIV1)
        static_assert(tDivider <= 15, "Divider cannot be greater than 15!");
        RCC->CFGR2 = ((RCC->CFGR2 & ~RCC_CFGR2_PREDIV1) | ((tDivider - 1) << RCC_CFGR2_PREDIV1_Pos));
#else
        static_assert(1 <= tDivider && tDivider <= 2, "Divider can be equal 1 or 2!");
        if constexpr (tDivider == 2)
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLXTPRE) | RCC_CFGR_PLLXTPRE_HSE_DIV2;
        }
        else
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLXTPRE) | RCC_CFGR_PLLXTPRE_HSE;
        }
#endif
    }

    template <uint32_t multiplier>
    void PLLClock::setMultiplier()
    {
#if !(defined(RCC_CFGR_PLLMULL3) && defined(RCC_CFGR_PLLMULL10))
        static_assert(4 <= multiplier && multiplier <= 9, "Multiplier can be equal 4..9!");
#else
        static_assert(4 <= multiplier && multiplier <= 16, "Multiplier cannot be greate than 16");
#endif
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PLLMULL) | ((multiplier - 2) << RCC_CFGR_PLLMULL_Pos);
    }

    template <uint32_t tDivider>
    void PLLClock::setSysOutputDivider() { /** no divide */ }

    template <uint32_t tDivider>
    void PLLClock::setUSBOutputDivider()
    {
        if constexpr (tDivider == 0u)
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_USBPRE) | RCC_CFGR_USBPRE;
        }
        else
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_USBPRE);
        }
    }

    template <uint32_t tDivider>
    void PLLClock::setI2SOutputDivider() { /** no divide */ }

    enum class SysClock::Source
    {
        HSI,
        HSE,
        PLL,
    };

    uint32_t SysClock::getFrequency()
    {
        return SystemCoreClock;
    }

    template <SysClock::Source tSource>
    void SysClock::selectSource()
    {
        uint32_t selectMask;
        uint32_t statusMask;

        if constexpr (tSource == SysClock::Source::HSI)
        {
            selectMask = RCC_CFGR_SW_HSI;
            statusMask = RCC_CFGR_SWS_HSI;
            SystemCoreClock = HSIClock::getFrequency();
        }
        else if constexpr (tSource == SysClock::Source::HSE)
        {
            selectMask = RCC_CFGR_SW_HSE;
            statusMask = RCC_CFGR_SWS_HSE;
            SystemCoreClock = HSEClock::getFrequency();
        }
        else if constexpr (tSource == SysClock::Source::PLL)
        {
            selectMask = RCC_CFGR_SW_PLL;
            statusMask = RCC_CFGR_SWS_PLL;
            SystemCoreClock = PLLClock::getFrequency();
        }

        uint32_t timeout = 10000;
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | selectMask;

        while (((RCC->CFGR & RCC_CFGR_SWS) != statusMask) && --timeout)
            asm volatile("nop");
    }

    class AHBClock : public BusClock<SysClock>
    {
    public:
        enum class Prescaler
        {
            DIV1 = RCC_CFGR_HPRE_DIV1,
            DIV2 = RCC_CFGR_HPRE_DIV2,
            DIV4 = RCC_CFGR_HPRE_DIV4,
            DIV8 = RCC_CFGR_HPRE_DIV8,
            DIV16 = RCC_CFGR_HPRE_DIV16,
            DIV64 = RCC_CFGR_HPRE_DIV64,
            DIV128 = RCC_CFGR_HPRE_DIV128,
            DIV256 = RCC_CFGR_HPRE_DIV256,
            DIV512 = RCC_CFGR_HPRE_DIV512,
        };

        template <Prescaller tPrescaller>
        static inline void setPrescaller()
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_HPRE) | static_cast<uint32_t>(tPrescaller);
        }
    };

    class APB1Clock : public BusClock<AHBClock>
    {
    public:
        enum class Prescaler
        {
            DIV1 = RCC_CFGR_PPRE1_DIV1,
            DIV2 = RCC_CFGR_PPRE1_DIV2,
            DIV4 = RCC_CFGR_PPRE1_DIV4,
            DIV8 = RCC_CFGR_PPRE1_DIV8,
            DIV16 = RCC_CFGR_PPRE1_DIV16,
        };

        template <Prescaller tPrescaller>
        static inline void setPrescaller()
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1) | static_cast<uint32_t>(tPrescaller);
        }
    };

    class APB2Clock : public BusClock<AHBClock>
    {
    public:
        enum class Prescaler
        {
            DIV1 = RCC_CFGR_PPRE2_DIV1,
            DIV2 = RCC_CFGR_PPRE2_DIV2,
            DIV4 = RCC_CFGR_PPRE2_DIV4,
            DIV8 = RCC_CFGR_PPRE2_DIV8,
            DIV16 = RCC_CFGR_PPRE2_DIV16,
        };

        template <Prescaller tPrescaller>
        static inline void setPrescaller()
        {
            RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2) | static_cast<uint32_t>(tPrescaller);
        }
    };
}
