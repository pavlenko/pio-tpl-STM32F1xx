#include <stm32f1xx_hal.h>

#include <string.h>

#include <Dispatcher.hpp>

#include <stm32cpp/Clock.hpp>
#include <stm32cpp/Delay.hpp>
#include <stm32cpp/I2C.hpp>
#include <stm32cpp/SPI.hpp>
#include <stm32cpp/IO.hpp>

#include "uart.hpp"
#include "cli.hpp"

volatile uint32_t delay = 500;

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

    Delay::init();

    // Led config
    IO::PC::enable();
    IO::PC13::configure<IO::Mode::OUTPUT>(IO::Speed::LOW);
    // Led config end

    UART1_Init();
    CLI_Init();

    while (true)
    {
        Dispatcher::dispatch();

        // Console::process(nullptr, 0);

        IO::PC13::tog();
        Delay::ms(delay);
        CLI::write("OK\n");
        Delay::ms(delay);
    }
    return 0;
}

extern "C" void SysTick_Handler(void)
{
    STM32::Delay::dispatchIRQ();
}
