#include <stm32f1xx_hal.h>

#include <string.h>

#include <Dispatcher.hpp>

#include <stm32cpp/Clock.hpp>
#include <stm32cpp/Delay.hpp>
#include <stm32cpp/IO.hpp>
#include <stm32cpp/UART.hpp>

#include "uart.hpp"
#include "cli.hpp"

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

    // Led config TODO move to Board::LED1{init,on,off}
    // TODO also get back delayed off logic, or create some new more usable
    IO::PC::enable();
    IO::PC13::configure<IO::Mode::OUTPUT>(IO::Speed::LOW);
    IO::PC13::set();
    // Led config end

    UART1::init();
    UART1::write("HELLO\n");

    while (true)
    {
        STM32::UART1_Driver::dispatch();

        //TODO change IRQ handling to push task to dispatcher for execute it in main loop!!!
        // Dispatcher::dispatch();

        Delay::ms(850);
        IO::PC13::tog();
        Delay::ms(150);
        IO::PC13::tog();

        Console::instance().write("OK\n");
        Console::instance().flush();
        // Delay::ms(delay);
    }
    return 0;
}

extern "C" void SysTick_Handler(void)
{
    STM32::Delay::dispatchIRQ();
}
