#ifndef __STM32_DELAY_HPP__
#define __STM32_DELAY_HPP__

#include <stdint.h>
#include "_common.hpp"

namespace STM32
{
    class Delay
    {
    private:
        static inline uint32_t _ms;

    public:
        static inline void init()
        {
            SysTick_Config(SystemCoreClock / 1000);
            NVIC_EnableIRQ(SysTick_IRQn);
        }

        static inline void ms(uint32_t ms)
        {
            _ms = ms;
            while (_ms > 0)
            {
                asm("nop");
            }
        }

        static inline void dispatchIRQ()
        {
            if (_ms > 0)
            {
                _ms--;
            }
        }
    };
}

#endif // __STM32_DELAY_HPP__