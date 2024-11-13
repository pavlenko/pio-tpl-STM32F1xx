#include <stm32/f1/delay.h>

static __IO uint32_t ms_ticks;

void SysTick_Handler()
{
    if (ms_ticks > 0) {
        ms_ticks--;
    }
}

void delay_setup(void)
{
    /* Update SystemCoreClock value */
    SystemCoreClockUpdate();

    /* Configure the SysTick timer to overflow every 1 ms */
    SysTick_Config(SystemCoreClock / 1000);
}

void delay_ms(uint32_t ms)
{
    ms_ticks = ms;
    while (ms_ticks);
}