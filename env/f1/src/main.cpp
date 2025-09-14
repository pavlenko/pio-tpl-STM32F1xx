#include <stm32f1xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>

int main(void)
{
    using namespace STM32;

    // Clock config
    Clock::HSEClock::on();

    Clock::PLLClock::configure<
        Clock::PLLClock::Source::HSE,
        Clock::PLLClockConfig<9u, 1u, false>
    >();
    Clock::PLLClock::on();

    // using SYSClockConfig = Clock::SYSClockConfig<
    //     Clock::SYSClockSource::PLL,
    //     RCC_CFGR_HPRE_DIV1,
    //     RCC_CFGR_PPRE1_DIV2,
    //     RCC_CFGR_PPRE2_DIV1,
    //     FLASH_LATENCY_2
    // >;
    Clock::SysClock::selectSource<Clock::SysClock::Source::PLL>();
    Clock::AHBClock::setPrescaler<Clock::AHBClock::Prescaler::DIV1>();
    Clock::APB1Clock::setPrescaler<Clock::APB1Clock::Prescaler::DIV2>();
    Clock::APB2Clock::setPrescaler<Clock::APB2Clock::Prescaler::DIV1>();
    //TODO flash latency
    //  Clock config end

    while (true)
    {
        asm("nop");
    }
    return 0;
}
