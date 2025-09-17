#define HSE_VALUE 8000000U

#include <stm32f4xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>

#include <stm32/lib/delay.hpp>

int main(void)
{
    using namespace STM32;

    // Clock config
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    Clock::HSEClock::on();
    Clock::PLLClock::configure<
        Clock::PLLClock::Source::HSE,
        Clock::PLLClockConfig<8u, 336u, 2u, 7u, 2u> //<-- HSE 8MHz -> SYS 168 MHz
    >();
    Clock::PLLClock::on();
    // ???
    Clock::AHBClock::setPrescaler<Clock::AHBClock::Prescaler::DIV1>();
    Clock::APB1Clock::setPrescaler<Clock::APB1Clock::Prescaler::DIV4>();
    Clock::APB2Clock::setPrescaler<Clock::APB2Clock::Prescaler::DIV2>();
    Clock::SysClock::selectSource<Clock::SysClock::Source::HSE>();
    Flash::configure(SystemCoreClock);
    //  Clock config end

    IO::PB::enable();
    IO::PB2::configure<IO::Config<IO::Mode::OUTPUT>>();

    Delay::init();

    while (true)
    {
        IO::PB2::tog();
        Delay::ms(500);
    }
    return 0;
}

extern "C" void SysTick_Handler(void)
{
    Delay::dispatchIRQ();
}
