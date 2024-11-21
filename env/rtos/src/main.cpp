#include <stm32f1xx_hal.h>

#include <FreeRTOS.h>
#include <task.h> /* RTOS task related API prototypes. */

#include <stm32cpp/Clock.hpp>
#include <stm32cpp/IO.hpp>

static void blink(void *argument)
{
    while (1)
    {
        STM32::IO::PC13::tog();
        vTaskDelay(500);
    }
}

int main(void)
{
    using namespace STM32;

    // Clock config
    Clock::HSEClock::on();

    Clock::PLLClock::configure<Clock::PLLClockConfig<Clock::PLLClockSource::HSE, RCC_CFGR_PLLMULL9, 0u>>();
    Clock::PLLClock::on();

    using SYSClockConfig = Clock::SYSClockConfig<
        Clock::SYSClockSource::PLL,
        RCC_CFGR_HPRE_DIV1,
        RCC_CFGR_PPRE1_DIV2,
        RCC_CFGR_PPRE2_DIV1,
        FLASH_LATENCY_2>;
    Clock::SYSClock::configure<SYSClockConfig>();
    // Clock config end

    // Led config
    IO::PC::enable();
    IO::PC13::configure<IO::Mode::OUTPUT>(IO::Speed::LOW);
    IO::PC13::set();
    // Led config end

    xTaskCreate(blink, "blink", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();

    while (true)
    {
        // TODO
    }
    return 0;
}
