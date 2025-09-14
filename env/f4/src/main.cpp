#define HSE_VALUE 8000000U

#include <stm32f4xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>

int main(void)
{
    using namespace STM32;

    // Clock config
    Clock::HSEClock::on();

    Clock::PLLClock::configure<
        Clock::PLLClock::Source::HSE,
        Clock::PLLClockConfig<8u, 336u, 2u, 7u, 2u> //<-- HSE 8MHz -> SYS 168 MHz
    >();
    Clock::PLLClock::on();
    Clock::SysClock::selectSource<Clock::SysClock::Source::PLL>();
    Clock::AHBClock::setPrescaler<Clock::AHBClock::Prescaler::DIV1>();
    Clock::APB1Clock::setPrescaler<Clock::APB1Clock::Prescaler::DIV4>();
    Clock::APB2Clock::setPrescaler<Clock::APB2Clock::Prescaler::DIV2>();
    //TODO flash latency
    //  Clock config end

    while (true)
    {
        asm("nop");
    }
    return 0;
}
