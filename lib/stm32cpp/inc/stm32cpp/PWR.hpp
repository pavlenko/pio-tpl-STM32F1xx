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

        enum class LPEntry{WFI,WFE};

        // see https://controllerstech.com/low-power-modes-in-stm32/
        class LPMode
        {
        public:
            // overall need to disable systick irq otherwise it will leave lp mode
            // also need enable it back on exit sleep mode, if use sleeponexit here need to disable it
            // on exit stop mode we also need configure clock again
            static inline void enter(LPType mode, LPEntry req)
            {
                //sleep:
                //- clear SLEEPDEEP
                //- call __WFI() or call __SEV();__WFE();__WFE();
                //stop:
                //- clear PDDS
                //- set LPDS value
                //- set SLEEPDEEP
                //- call __WFI() or call __SEV();__WFE();__WFE();
                //- clear SLEEPDEEP
                //standby:
                //--disable wakeup flags
                //--enable wakeup pin or RTC periodic wakeup
                //- set PDDS
                //- set SLEEPDEEP
                //- call __WFI()
            }
        };
    }
}

#endif // __STM32_PWR_HPP__