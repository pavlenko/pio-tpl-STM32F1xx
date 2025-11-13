#include <stm32f0xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>
#include <stm32/dev/i2c.hpp>

#include <stm32/lib/delay.hpp>

int main(void)
{
    using namespace STM32;

    // Clock config
    Clock::HSEClock::on();
    Clock::PLLClock::configure<
        Clock::PLLClock::Source::HSE_PREDIV,
        Clock::PLLClockConfig<5u, 0u, false> //<-- HSE 8MHz -> SYS 48 MHz
    >();
    Clock::PLLClock::on();

    Clock::SysClock::configure<
        Clock::SysClock::Source::PLL,
        Flash::Latency::WS1,
        Clock::SysClockConfig<Clock::AHBClock::Divider::DIV1, Clock::APBClock::Divider::DIV1>
    >();
    //  Clock config end

    IO::PC::enable();
    IO::PC13::configure<IO::Config<IO::Mode::OUTPUT>>();

    Delay::init();

    while (true)
    {
        IO::PB2::tog();
        Delay::ms(500);
    }
    return 0;
}
