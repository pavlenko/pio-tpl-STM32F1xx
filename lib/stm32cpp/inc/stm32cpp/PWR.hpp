#ifndef __STM32_PWR_HPP__
#define __STM32_PWR_HPP__

#include <cstdint>

extern "C"
{
#include "stm32f1xx_hal_pwr.h"
}

/**
 * @brief Power control
 *
 * see https://controllerstech.com/low-power-modes-in-stm32/
 */
namespace STM32
{
    namespace Power
    {
        namespace BKPAccess
        {
            static inline void enable() { PWR->CR |= PWR_CR_DBP; }
            static inline void disable() { PWR->CR &= ~PWR_CR_DBP; }
        }

        class PVD
        {
            // TODO enable/disable/config/irq
        };

        enum class LPEntry
        {
            WFI,
            WFE
        };

        class LPMode
        {
        public:
            template <LPEntry tEntry>
            static inline void enterSLEEP()
            {
                SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;

                if constexpr (tEntry == LPEntry::WFI) // Request wait for interrupt
                {
                    __WFI();
                }
                else // Request wait for event sequence
                {
                    __SEV();
                    __WFE();
                    __WFE();
                }
            }

            template <LPEntry tEntry>
            static inline void enterSTOP(bool lpEnabled = false)//TODO pass enum
            {
                PWR->CR &= ~PWR_CR_PDDS;
                MODIFY_REG(PWR->CR, PWR_CR_LPDS, lpEnabled << PWR_CR_LPDS_Pos);
                SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

                if constexpr (tEntry == LPEntry::WFI) // Request wait for interrupt
                {
                    __WFI();
                }
                else // Request wait for event sequence
                {
                    __SEV();
                    __WFE();
                    __WFE();
                }

                SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
            }

            static inline void enterSTANDBY(void)
            {
                PWR->CR |= PWR_CR_PDDS;
                SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
                __WFI(); // Request wait for interrupt
            }
        };
    }
}

#endif // __STM32_PWR_HPP__