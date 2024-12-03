#pragma once

#include <stdint.h>
#include "_common.hpp"

namespace STM32
{
    class SYSTick
    {
    private:
        static inline uint32_t m_ticks{0};

    public:
        static inline void init()
        {
            // As of system core clock in MHz we just divide it by 1000 for get 1ms per tick
            SysTick_Config(SystemCoreClock / 1000);
            NVIC_EnableIRQ(SysTick_IRQn);
        }

        static inline uint32_t getTicks()
        {
            return m_ticks;
        }

        static inline void delay(uint32_t ms)
        {
            uint32_t tmp = m_ticks + ms;
            while (m_ticks < tmp)
            {
                asm("nop");
            }
        }

        static inline void dispatchIRQ()
        {
            m_ticks++;
        }
    };
}
