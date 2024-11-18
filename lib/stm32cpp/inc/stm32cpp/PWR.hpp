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
            static inline void enable()
            {
                __HAL_RCC_PWR_CLK_ENABLE();
                PWR->CR |= PWR_CR_DBP;
            }
            static inline void disable()
            {
                PWR->CR &= ~PWR_CR_DBP;
            }
        }

        // F1,F4: 2.2...2.9
        // F2: 2.0...2.9
        // L0: 1.9...3.1
        enum class PVDLevel
        {
            _0, // maybe alias
            _1,
            _2,
            _3,
            _4,
            _5,
            _6,
            _7,
        };

        enum class PVDMode
        {
            NORMAL,
            // External IRQ
            EXTI_RISING, // TODO values??? maybe better split to irq/event & exti edge
            EXTI_FALLING,
            EXTI_BOTH,
            // Event
            EVENT_RISING,
            EVENT_FALLING,
            EVENT_BOTH,
        };

        inline PVDMode operator|(PVDMode lft, PVDMode rgt)
        {
            return PVDMode(static_cast<uint32_t>(lft) | static_cast<uint32_t>(rgt));
        }

        namespace PVD
        {
            static inline void configure()
            {
                // level affects PWR regs
                // mode affects EXTI regs
            }
            static inline void enable()
            {
            }
            static inline void disable()
            {
            }
            static inline void dispatchIRQ()
            {
            }
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
            static inline void enterSTOP(bool lpEnabled = false) // TODO pass enum
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