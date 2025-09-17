#include <stm32f1xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>

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
    Clock::AHBClock::setDivider<Clock::AHBClock::Divider::DIV1>();
    Clock::APB1Clock::setDivider<Clock::APB1Clock::Divider::DIV2>();
    Clock::APB2Clock::setDivider<Clock::APB2Clock::Divider::DIV1>();
    Flash::configure(SystemCoreClock);
    //  Clock config end

    IO::PC::enable();
    //IO::PC13::configure(/*mode, speed, pull, otype, af*/);

    while (true)
    {
        asm("nop");
    }
    return 0;
}
