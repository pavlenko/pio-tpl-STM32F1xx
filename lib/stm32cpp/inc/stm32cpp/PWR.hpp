#ifndef __STM32_PWR_HPP__
#define __STM32_PWR_HPP__

#include <cstdint>

extern "C"
{
#include "stm32f1xx_hal_pwr.h"
}

namespace STM32
{
    namespace Power
    {
        class PVD
        {
            // enable/disable/config/irq
        };
        // backup: enable/disable access

        enum class LPType
        {
            SLEEP,
            STOP,
            STANDBY,
        };

        enum class LPEntry
        {
            WFI,
            WFE
        };

        // see https://controllerstech.com/low-power-modes-in-stm32/
        class LPMode
        {
        public:
            // overall need to disable systick irq otherwise it will leave lp mode
            // also need enable it back on exit sleep mode, if use sleeponexit here need to disable it
            // on exit stop mode we also need configure clock again
            static inline void enter(LPType mode, LPEntry req)
            {
                // sleep:
                //- clear SLEEPDEEP
                //- call __WFI() or call __SEV();__WFE();__WFE();
                // stop:
                //- clear PDDS
                //- set LPDS value
                //- set SLEEPDEEP
                //- call __WFI() or call __SEV();__WFE();__WFE();
                //- clear SLEEPDEEP
                // standby:
                //--disable wakeup flags
                //--enable wakeup pin or RTC periodic wakeup
                //- set PDDS
                //- set SLEEPDEEP
                //- call __WFI()
            }

            template <LPEntry tEntry>
            static inline void sleep()
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
            static inline void stop(bool lpEnabled = false)
            {
                PWR->CR &= ~PWR_CR_PDDS;
                MODIFY_REG(PWR->CR, PWR_CR_LPDS, lpEnabled);
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

            static inline void standby(void)
            {
                PWR->CR |= PWR_CR_PDDS;
                SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
                __WFI(); // Request wait for interrupt
            }
        };
    }
}

#endif // __STM32_PWR_HPP__