#include <stm32f1xx_hal.h>

#include <string.h>

#include <stm32/dev/clock.hpp>
#include <stm32/dev/flash.hpp>
#include <stm32/dev/io.hpp>
#include <stm32/dev/i2c.hpp>
#include <stm32/dev/uart.hpp>

#include <stm32/lib/delay.hpp>

using namespace STM32;
using LED = IO::PC13;

static uint8_t rxBuf[255];

int main(void)
{
    // Clock config
    Clock::HSEClock::on();
    // HSE 8MHz -> SYS 72 MHz
    Clock::PLLClock::configure<Clock::PLLClock::Source::HSE, Clock::PLLClockConfig<9u, 1u, false>>();
    Clock::PLLClock::on();

    Clock::SysClock::configure<Clock::SysClock::Source::PLL, Flash::Latency::WS2, Clock::SysClockConfig<Clock::AHBClock::Divider::DIV1, Clock::APB1Clock::Divider::DIV2, Clock::APB2Clock::Divider::DIV1>>();
    //  Clock config end

    LED::port::enable();
    LED::configure<IO::Config<IO::Mode::OUTPUT>>();

    UART1::configure<9600u, UART::Config::ENABLE_RX_TX>();
    UART1::rxDMA(rxBuf, 255, [](DMA::Event e) {});

    Delay::init();

    while (true) {
        LED::tog();
        Delay::ms(500);
    }
    return 0;
}

extern "C" void SysTick_Handler(void)
{
    Delay::dispatchIRQ();
}

extern "C" void DMA1_Channel4_IRQHandler(void)
{
    UART1::DMATx::dispatchIRQ();
}

extern "C" void DMA1_Channel5_IRQHandler(void)
{
    UART1::DMARx::dispatchIRQ();
}