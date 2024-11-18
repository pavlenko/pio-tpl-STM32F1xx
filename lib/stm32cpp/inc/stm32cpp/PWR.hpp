#ifndef __STM32_PWR_HPP__
#define __STM32_PWR_HPP__

#include <cstdint>

#include <stm32cpp/EXTI.hpp>

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
            IRQ = 0x1,
            EVENT,
            BOTH,
        };

        inline PVDMode operator|(PVDMode lft, PVDMode rgt)
        {
            return PVDMode(static_cast<uint32_t>(lft) | static_cast<uint32_t>(rgt));
        }

        inline PVDMode operator&(PVDMode lft, PVDMode rgt)
        {
            return PVDMode(static_cast<uint32_t>(lft) & static_cast<uint32_t>(rgt));
        }

        typedef void (*PVDCallback)();

        namespace PVD
        {
            template <PVDLevel tLevel, PVDMode tMode, EXTIEdge tEdge>
            static inline void configure()
            {
                MODIFY_REG(PWR->CR, PWR_CR_PLS, static_cast<uint32_t>(tLevel) << PWR_CR_PLS_Pos);

                EXTI->IMR &= ~PWR_EXTI_LINE_PVD;
                EXTI->EMR &= ~PWR_EXTI_LINE_PVD;
                EXTI->RTSR &= ~PWR_EXTI_LINE_PVD;
                EXTI->FTSR &= ~PWR_EXTI_LINE_PVD;

                if constexpr (tMode == PVDMode::IRQ || tMode == PVDMode::BOTH)
                {
                    EXTI->IMR |= PWR_EXTI_LINE_PVD;
                }
                if constexpr (tMode == PVDMode::EVENT || tMode == PVDMode::BOTH)
                {
                    EXTI->EMR |= PWR_EXTI_LINE_PVD;
                }
                if constexpr (tEdge == EXTIEdge::RISING || tEdge == EXTIEdge::BOTH)
                {
                    EXTI->RTSR |= PWR_EXTI_LINE_PVD;
                }
                if constexpr (tEdge == EXTIEdge::FALLING || tEdge == EXTIEdge::BOTH)
                {
                    EXTI->FTSR |= PWR_EXTI_LINE_PVD;
                }
            }
            static inline void enable()
            {
                PWR->CR |= PWR_CR_PVDE;
            }
            static inline void disable()
            {
                PWR->CR &= ~PWR_CR_PVDE;
            }
            static inline void dispatchIRQ()
            {
                if (EXTI->PR & PWR_EXTI_LINE_PVD)
                {
                    EXTI->PR |= PWR_EXTI_LINE_PVD;
                    //TODO callback
                }
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