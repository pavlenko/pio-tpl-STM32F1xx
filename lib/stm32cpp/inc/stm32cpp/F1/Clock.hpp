#ifndef __STM32_CLOCK_F1_HPP__
#define __STM32_CLOCK_F1_HPP__

#if defined(STM32F1)

#include <stm32cpp/common/Clock.hpp>
#include <stm32cpp/common/Flash.hpp>

namespace STM32
{
    namespace Clock
    {
        // Private
        namespace
        {
            volatile uint32_t PLLClockFrequency{ 0u };
        }

        // PLLCLK
        template <PLLClockSource tSource, uint32_t tPLLMul, uint32_t tPLLDiv>
        struct PLLClockConfig
        {
            static constexpr auto Source = tSource;
            static constexpr auto PLLMul = tPLLMul;
            static constexpr auto PLLDiv = tPLLDiv;
        };

        template <typename T>
        void PLLClock::_update()
        {
            static constexpr const uint32_t pllmul = (T::PLLMul >> RCC_CFGR_PLLMULL_Pos) + 2u;

            if constexpr (T::Source == PLLClockSource::HSI) {
                PLLClockFrequency = (HSIClock::getFrequency() >> 1u) * pllmul;
            }
            if constexpr (T::Source == PLLClockSource::HSE) {
                if constexpr (T::PLLDiv != 0u) {
                    PLLClockFrequency = (HSIClock::getFrequency() >> 1u) * pllmul;
                }
                else {
                    PLLClockFrequency = HSIClock::getFrequency() * pllmul;
                }
            }
        }

        template <typename T>
        void PLLClock::_configure()
        {
            static constexpr uint32_t source = static_cast<uint32_t>(T::Source) << RCC_CFGR_PLLSRC_Pos;
            static constexpr uint32_t pllmul = static_cast<uint32_t>(T::PLLMul);
            static constexpr uint32_t plldiv = static_cast<uint32_t>(T::PLLDiv);

            if constexpr (T::Source == PLLClockSource::HSE) {
                __HAL_RCC_HSE_PREDIV_CONFIG(plldiv);
            }

            __HAL_RCC_PLL_CONFIG(source, pllmul);
        }

        uint32_t PLLClock::getFrequency()
        {
            return PLLClockFrequency;
        }

        // SYSCLK
        template <SYSClockSource tSource, uint32_t tAHBDiv, uint32_t tAPB1Div, uint32_t tAPB2Div, uint32_t tFLatency>
        struct SYSClockConfig
        {
            static constexpr auto Source = tSource;
            static constexpr auto AHBDiv = tAHBDiv;
            static constexpr auto APB1Div = tAPB1Div;
            static constexpr auto APB2Div = tAPB2Div;
            static constexpr auto FLatency = tFLatency;
        };

        template <typename T>
        void SYSClock::_configure()
        {
            static constexpr const auto source = static_cast<uint32_t>(T::Source);

            if (T::FLatency > Flash::getLatency()) {
                Flash::setLatency(T::FLatency);
            }

            MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, (T::AHBDiv & RCC_CFGR_HPRE));
            MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, (T::APB1Div & RCC_CFGR_PPRE1));
            MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, (T::APB2Div & RCC_CFGR_PPRE2));

            MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, (source << RCC_CFGR_SW_Pos));

            while ((RCC->CFGR & RCC_CFGR_SWS) != (source << RCC_CFGR_SWS_Pos));

            if (T::FLatency < Flash::getLatency()) {
                Flash::setLatency(T::FLatency);
            }
        }

        // RTCCLK
        template <RTCClockSource tSource>
        struct RTCClockConfig
        {
            static constexpr auto source = static_cast<uint32_t>(tSource);
        };

        uint32_t RTCClock::getFrequency()
        {
            switch (RCC->BDCR & RCC_BDCR_RTCSEL) {
            case RCC_BDCR_RTCSEL_LSI:
                return LSEClock::getFrequency();
            case RCC_BDCR_RTCSEL_LSE:
                return LSIClock::getFrequency();
            case RCC_BDCR_RTCSEL_HSE:
                return HSEClock::getFrequency() / 128u;
            default:
                return 0;
            }
        }

        template <typename T>
        void RTCClock::configure()
        {
            __HAL_RCC_RTC_CONFIG(T::source);
        }
    }
}

#endif

#endif // __STM32_CLOCK_F1_HPP__
