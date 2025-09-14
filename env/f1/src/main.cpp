#include <stm32f1xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>

int main(void)
{
    using namespace STM32;

    // Clock config
    Clock::HSEClock::on();

    Clock::PLLClock::configure<
        Clock::PLLClock::Source::HSE,
        Clock::PLLClockConfig<9u, 1u, false> //<-- HSE 8MHz -> SYS 72 MHz
    >();
    Clock::PLLClock::on();
    Clock::SysClock::selectSource<Clock::SysClock::Source::PLL>();
    Clock::AHBClock::setPrescaler<Clock::AHBClock::Prescaler::DIV1>();
    Clock::APB1Clock::setPrescaler<Clock::APB1Clock::Prescaler::DIV2>();
    Clock::APB2Clock::setPrescaler<Clock::APB2Clock::Prescaler::DIV1>();
    Flash::configure(SystemCoreClock);
    //  Clock config end

    while (true)
    {
        asm("nop");
    }
    return 0;
}
